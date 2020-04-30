#include <iostream>
#include <experimental/filesystem>
#include <memory>
#include "args.hxx"
//#include "git2.h"
//#include "git2/repository.h"


namespace fs = std::experimental::filesystem;

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


//    git_libgit2_init();
//
//    git_object *treeish = nullptr;
//    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
//
//    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
//    git_repository *repo = nullptr;
//    if(git_repository_open_ext(&repo, fs::current_path().c_str(), GIT_REPOSITORY_OPEN_FROM_ENV,nullptr) < 1) {
//
//        fprintf(stderr, "Could not open repository: %s\n", git_error_last()->message);
//        exit(1);
//    }
//    std::cout << std::string(repo->workdir) << std::endl;
//
//    git_revparse_single(&treeish, repo, "master");
//    git_checkout_tree(repo, treeish, &opts);
//    git_checkout_head(repo,&opts);
//
//    git_repository_set_head(repo, "refs/heads/master");
//
//    git_object_free(treeish);
//    git_libgit2_shutdown();


    return 0;
}