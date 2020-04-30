//#define DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT

#include <unordered_map>
#include <iostream>
#include <optional>
#include "fts_fuzzy_match.h"
#include "git2.h"
#include "git2cpp/initializer.h"
#include "git2cpp/repo.h"
#include "git2cpp/annotated_commit.h"
#include "git2/errors.h"
#include "boost/filesystem.hpp"
#include "doctest/doctest.h"


void perform_checkout_ref(git::Repository &repo, git::AnnotatedCommit const &target) {
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

    /** Setup our checkout options from the parsed options */
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;


    /** Grab the commit we're interested to move to */
    auto target_commit = repo.commit_lookup(target.commit_id());
    /**
     * Perform the checkout so the workdir corresponds to what target_commit
     * contains.
     *
     * Note that it's okay to pass a git_commit here, because it will be
     * peeled to a tree.
     */
    if (repo.checkout_tree(target_commit, checkout_opts)) {
        fprintf(stderr, "failed to checkout tree: %s\n", git_error_last()->message);
        return;
    }

    /**
     * Now that the checkout has completed, we have to update HEAD.
     *
     * Depending on the "origin" of target (ie. it's an OID or a branch name),
     * we might need to detach HEAD.
     */
    int err;
    if (auto ref = target.commit_ref()) {
        err = repo.set_head(ref);
    } else {
        err = repo.set_head_detached(target);
    }
    if (err != 0) {
        fprintf(stderr, "failed to update HEAD reference: %s\n", git_error_last()->message);
    }
}

git::AnnotatedCommit resolve_refish(git::Repository const &repo, const char *refish) {
    if (auto ref = repo.dwim(refish)) {
        return repo.annotated_commit_from_ref(ref);
    }

    auto obj = repo.revparse_single(refish);
    return repo.annotated_commit_lookup(obj.single()->id());
}

bool contains_only_digits(const std::string &input) {
    return std::all_of(input.begin(), input.end(), isdigit);
}

TEST_CASE ("contains_only_digits") {
            CHECK(contains_only_digits("123"));
            CHECK(!contains_only_digits("123a"));
            CHECK(!contains_only_digits("a123"));
            CHECK(!contains_only_digits("abcz"));
            CHECK(contains_only_digits(""));
}

void print_usage() {
// TODO
}

std::optional<std::string>
find_matching_branch(const std::string &input, const std::vector<std::string> &branch_names) {
    constexpr const int MIN_MATCHING_SCORE = 50;
    if (input.empty()) {
        return std::nullopt;
    }

    struct FuzzyResult {
        std::string &name;
        int score;
    };
    const std::string &empty = "";
    FuzzyResult best_match{const_cast<std::string &>(empty), 0};

    for (const std::string &branch_name : branch_names) {
        int score;
        bool pattern_found = fts::fuzzy_match(input.c_str(), branch_name.c_str(), score);
        if (!pattern_found) {
            continue;
        }

        if (score > best_match.score) {
            best_match.score = score;
            best_match.name = branch_name;
        }
    }

    if (best_match.score < MIN_MATCHING_SCORE) {
        return std::nullopt;
    }
    return best_match.name;

}

TEST_CASE ("find_matching_branch") {
            SUBCASE("test matching") {
        std::vector<std::string> branch_names = {"ab123", "123", "abc45"};
                CHECK(find_matching_branch("123", branch_names).value() == "123");
                CHECK(!find_matching_branch("", branch_names).has_value());
                CHECK(!find_matching_branch("def", branch_names).has_value());
                CHECK(find_matching_branch("abc", branch_names).value() == "abc45");
    }
            SUBCASE("test fuzzyness") {
        std::vector<std::string> branch_names = {"master", "masterabc", "abcmaster"};
                CHECK(find_matching_branch("ma", branch_names).value() == "master");
                CHECK(find_matching_branch("mac", branch_names).value() == "masterabc");
                CHECK(find_matching_branch("cr", branch_names).value() == "abcmaster");
    }

}

struct PartitionedBranchNames {
    std::vector<std::string> remotes;
    std::vector<std::string> locales;
};

PartitionedBranchNames extract_branch_names_from(const git::Repository &repo) {
    PartitionedBranchNames result;

    auto get_branch_names_of_type = [&](const git::branch_type &type) {
        const auto &branches = repo.branches(type);
        std::vector<std::string> branch_names;
        branch_names.reserve(branches.size());
        std::transform(branches.begin(), branches.end(), std::back_inserter(branch_names), [](const auto &branch) {
            return branch.name();
        });
        return branch_names;
    };
    result.remotes = get_branch_names_of_type(git::branch_type::REMOTE);
    result.locales = get_branch_names_of_type(git::branch_type::LOCAL);

    return result;
}


void git_fetch() {

}

int main(int argc, char **argv) {

    doctest::Context context;
    int res = context.run();

    if (context.shouldExit()) {
        return res;
    }

    if (argc != 2) {
        return 1;
    }
    const std::unordered_map<std::string, std::string> commands = {
            {"m",      "master"},
            {"master", "master"},
            {"d",      "devel"},
            {"devel",  "devel"}
    };

    std::string input(argv[1]);
    std::string branch_name;


    if (commands.find(input) != commands.end()) {
        branch_name = commands.at(input);
    }


    branch_name = input;


    auto_git_initializer;
    git::Repository repo(boost::filesystem::current_path().c_str());
    const auto[remote_branch_names, local_branch_names] = extract_branch_names_from(repo);

    const auto matching_local_branch = find_matching_branch(branch_name, local_branch_names);
    const auto matching_remote_branch = find_matching_branch(branch_name, remote_branch_names);


    if (!matching_local_branch && !matching_remote_branch) {
        std::cout << "Your input '" << branch_name << "' did not match any branch names.\nAvailable branches are:\n";
        for (const auto &branch : repo.branches(git::branch_type::LOCAL)) {
            // TODO beautify name
            std::cout << branch.name() << "\n";
        }
        return 1;
    }

    if (!matching_local_branch && matching_remote_branch) {
        git_fetch();
    }
    std::cout << "input " << branch_name << std::endl;
    std::cout << "local branch " << matching_local_branch.value_or("") << std::endl;
    std::cout << "remote branch " << matching_remote_branch.value_or("") << std::endl;


    auto checkout_target = resolve_refish(repo, matching_local_branch.value_or("master").c_str());
    perform_checkout_ref(repo, checkout_target);


    return res;
}