#include <iostream>
#include <experimental/filesystem>
#include <memory>
#include "args.hxx"
#include "git2cpp/repo.h"
#include "git2cpp/initializer.h"
#include "git2cpp/annotated_commit.h"
#include <git2/errors.h>

namespace fs = std::experimental::filesystem;
void perform_checkout_ref(git::Repository & repo, git::AnnotatedCommit const & target)
{
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
    if (repo.checkout_tree(target_commit, checkout_opts))
    {
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
git::AnnotatedCommit resolve_refish(git::Repository const & repo, const char *refish)
{
    if (auto ref = repo.dwim(refish))
        return repo.annotated_commit_from_ref(ref);

    auto obj = repo.revparse_single(refish);
    return repo.annotated_commit_lookup(obj.single()->id());
}
int main(int argc, char **argv) {
    args::ArgumentParser parser("This is a test program.", "This goes after the options.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<int> integer(parser, "integer", "The integer flag", {'i'});
    args::ValueFlagList<char> characters(parser, "characters", "The character flag", {'c'});
    args::Positional<std::string> foo(parser, "foo", "The foo position");
    args::PositionalList<double> numbers(parser, "numbers", "The numbers position list");
    try {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Help &) {
        std::cout << parser;
        return 0;
    }
    catch (const args::ParseError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (const args::ValidationError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    if (integer) { std::cout << "i: " << args::get(integer) << std::endl; }
    if (characters) { for (const auto ch: args::get(characters)) { std::cout << "c: " << ch << std::endl; }}
    if (foo) { std::cout << "f: " << args::get(foo) << std::endl; }
    if (numbers) { for (const auto nm: args::get(numbers)) { std::cout << "n: " << nm << std::endl; }}


    auto_git_initializer;
    git::Repository repo(fs::current_path().c_str());
    auto checkout_target = resolve_refish(repo, "master");

    perform_checkout_ref(repo, checkout_target);


    return 0;
}