#define DOCTEST_CONFIG_IMPLEMENT

#include <iostream>
#include "git2.h"
#include "git2cpp/initializer.h"
#include "git2cpp/repo.h"
#include "boost/filesystem.hpp"
#include "doctest/doctest.h"

#ifdef NDEBUG
#define DOCTEST_CONFIG_DISABLE
#endif

int main() {
    doctest::Context context;
    int res = context.run();

    if (context.shouldExit()) {
        return res;
    }


    auto_git_initializer;
    git::Repository repo(boost::filesystem::current_path().c_str());
    std::cout << repo.path() << std::endl;

    return res;
}


TEST_CASE ("testing the factorial function") {
            CHECK(2 == 1);


}

//#include <memory>
//#include <unordered_map>
//#include "git2cpp/repo.h"
//#include "git2cpp/initializer.h"
//#include "git2cpp/annotated_commit.h"
//#include <git2/errors.h>
//#include <optional>
//#include <regex>
//#include <boost/filesystem.hpp>
//

//
//
//void perform_checkout_ref(git::Repository &repo, git::AnnotatedCommit const &target) {
//    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
//
//    /** Setup our checkout options from the parsed options */
//    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
//
//
//    /** Grab the commit we're interested to move to */
//    auto target_commit = repo.commit_lookup(target.commit_id());
//    /**
//     * Perform the checkout so the workdir corresponds to what target_commit
//     * contains.
//     *
//     * Note that it's okay to pass a git_commit here, because it will be
//     * peeled to a tree.
//     */
//    if (repo.checkout_tree(target_commit, checkout_opts)) {
//        fprintf(stderr, "failed to checkout tree: %s\n", git_error_last()->message);
//        return;
//    }
//
//    /**
//     * Now that the checkout has completed, we have to update HEAD.
//     *
//     * Depending on the "origin" of target (ie. it's an OID or a branch name),
//     * we might need to detach HEAD.
//     */
//    int err;
//    if (auto ref = target.commit_ref()) {
//        err = repo.set_head(ref);
//    } else {
//        err = repo.set_head_detached(target);
//    }
//    if (err != 0) {
//        fprintf(stderr, "failed to update HEAD reference: %s\n", git_error_last()->message);
//    }
//}
//
//git::AnnotatedCommit resolve_refish(git::Repository const &repo, const char *refish) {
//    if (auto ref = repo.dwim(refish)) {
//        return repo.annotated_commit_from_ref(ref);
//    }
//
//    auto obj = repo.revparse_single(refish);
//    return repo.annotated_commit_lookup(obj.single()->id());
//}
//
//bool contains_only_digits(const std::string &input) {
//    return std::all_of(input.begin(), input.end(), isdigit);
//}
//
//void print_usage() {
//// TODO
//}
//
//std::optional<std::string>
//find_matching_branch(const std::string &input, const std::vector<std::string> &branch_names) {
//
//
//    for (const auto &branch_name : branch_names) {
//        if (branch_name.find(input) != std::string::npos) {
//            return branch_name;
//        }
//    }
//
//    return std::nullopt;
//}
//
//int main(int argc, char **argv) {
//
//    if (argc != 2) {
//        return 1;
//    }
//    std::unordered_map<std::string, std::string> commands = {
//            {"m",      "master"},
//            {"master", "master"},
//            {"d",      "devel"},
//            {"devel",  "devel"}
//    };
//
//    std::string input(argv[1]);
//    std::string branch_name;
//
//
//    if (commands.find(input) != commands.end()) {
//        branch_name = commands[input];
//    }
//
//    if (!contains_only_digits(input)) {
//        print_usage();
//        return 1;
//    }
//
//    branch_name = input;
//
//
//    auto_git_initializer;
//    git::Repository repo(boost::filesystem::current_path().c_str());
//
//
//    const auto &branches = repo.branches(git::branch_type::ALL);
//    std::vector<std::string> branch_names;
//    branch_names.reserve(branches.size());
//    std::transform(branches.begin(), branches.end(), std::back_inserter(branch_names), [](const auto &branch) {
//        return branch.name();
//    });
//
//    branch_name = "123";
//
//    auto partition_point = std::partition(branch_names.begin(), branch_names.end(), [](const std::string &name) {
//        return name.find("remote") != std::string::npos;
//    });
//    std::vector<std::string> remote_branch_names(branch_names.begin(), partition_point);
//    std::vector<std::string> local_branch_names(partition_point, branch_names.end());
//
//
//    std::cout << "Lokal branches" << std::endl;
//    for (const auto &b: local_branch_names) {
//        std::cout << b << std::endl;
//    }
//
//    std::cout << "Remote branches" << std::endl;
//    for (const auto &b: remote_branch_names) {
//        std::cout << b << std::endl;
//    }
//    auto matching_branch = find_matching_branch(branch_name, local_branch_names);
//    std::cout << "Found: " << matching_branch.value_or("NOTHING") << std::endl;
//
//
//
//
////    auto checkout_target = resolve_refish(repo, branch_name.c_str());
////    perform_checkout_ref(repo, checkout_target);
//
//
//    return 0;
//}