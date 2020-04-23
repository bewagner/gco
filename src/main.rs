extern crate clap;

use clap::{Arg, App};
use git2::Repository;
use regex::Regex;
use std::process::Command;
use std::process;
use std::error::Error;


fn main() {
    let command_name = "gco";
    let matches = App::new(command_name.to_owned() + " - Checkout think JIRA task branches")
        .version("1.0.0")
        .author("Benjamin Wagner")
        .about("Checkout think JIRA task branches by their TCD number")
        .arg(
            Arg::with_name("TCD number")
                .help("TCD number of task branch to check out")
                .index(1),
        )
        .subcommand(
            App::new("master")
                .about("Checkout master branch"),
        )
        .get_matches();

    abort_if_command_is_not_run_in_git_repo(command_name);

    let tcd_number_string = match matches.value_of("TCD number") {
        Some(o) => o,
        None => {
            eprintln!("You did not provide any input!");
            eprintln!("Run '{} --help' to get help.", command_name);
            process::exit(1);
        }
    };

    let tcd_number: u32 = match tcd_number_string.parse() {
        Ok(n) => n,
        Err(_) => {
            eprintln!("Could not parse your TCD number input. Make sure it only contains numbers. \nInput was: {}", tcd_number_string);
            eprintln!("Run '{} --help' to get help.", command_name);
            process::exit(1);
        }
    };


    get_git_branches(tcd_number);


    process::exit(0);
}

fn abort_if_command_is_not_run_in_git_repo(command_name: &str) {
    if let Err(_) = Repository::open_from_env() {
        eprintln!("The {} command only works inside of git repositories.", command_name);
        process::exit(1);
    };
}

fn get_remote_branches(repo: &Repository) -> Vec<git2::Branch> {
    let mut branches = Vec::new();
    for b in repo.branches(Some(git2::BranchType::Remote)) {
        branches = b
            .filter_map(|x| x.ok())
            .map(|x| x.0)
            .collect();
    }
    branches
}

fn filter_branch_names_by_regex(branches: &[git2::Branch], regex: &Regex) -> Vec<String> {
    branches.iter()
        .map(|x| x.name())
        .filter_map(|x| x.ok().unwrap())
        .filter(|x| regex.is_match(x))
        .map(|x| x.to_string())
        .collect()
}

fn get_filtered_branch_names(repo: &Repository, regex: &Regex) -> Vec<String> {
    let branches = get_remote_branches(&repo);
    filter_branch_names_by_regex(&branches, &regex)
}

fn extract_origin_path(branch_names: Vec<String>) -> Vec<String> {
    let mut branch_names = branch_names.clone();
    for branch_name in branch_names {
        if branch_name.contains("origin/") {
            branch_name = branch_name.split("origin/")
                .take(1)
                .collect::<String>()
        }
    }
    branch_names
}

fn get_git_branches(task_number: u32) -> Result<String, Box<dyn Error>> {
    let repo = Repository::open_from_env()?;
    let regex = task_number_regex(task_number);


    let branch_names = get_filtered_branch_names(&repo, &regex);

    // If no branches matched the regex, run 'git fetch' and try again in case the user missed to fetch the remote branch.
    if branch_names.is_empty() {
        git_fetch();
        let branch_names = get_filtered_branch_names(&repo, &regex);
    }

    for branch in branch_names {
        println!("{:#?}", branch);
    }


    // checkout_branch(&repo, "master");


    Ok("Hi".to_string())
}

fn task_number_regex(branch_number: u32) -> Regex {
    let search_string = format!(r"\b{}\b", branch_number.to_string());
    let regex = Regex::new(&search_string).unwrap();
    regex
}

/// Checkout a git branch
/// Solution taken from
/// https://stackoverflow.com/questions/55141013/how-to-get-the-behaviour-of-git-checkout-in-rust-git2
fn checkout_branch(mut repo: &Repository, branch_name: &str) {
    let head = repo.head().unwrap();
    let oid = head.target().unwrap();
    let target_commit = repo.find_commit(oid).unwrap();

    let branch = repo.branch(
        branch_name,
        &target_commit,
        false,
    );

    let tree = repo.revparse_single(&("refs/heads/".to_owned() +
        branch_name)).unwrap();

    repo.checkout_tree(
        &tree,
        None,
    ).unwrap();

    repo.set_head(&("refs/heads/".to_owned() + branch_name)).unwrap();
}

fn git_fetch() -> Result<(), Box<dyn Error>> {
    process::Command::new("git").arg("fetch").output()?;
    Ok(())
}


#[cfg(test)]
mod tests {
    use crate::{task_number_regex, extract_origin_path};

    #[test]
    fn regex_finds_branches() {
        let regex = task_number_regex(123);
        assert_eq!(true, regex.is_match("task-123-description"));
        assert_ne!(true, regex.is_match("task-123abc-description"));
        assert_ne!(true, regex.is_match("task-abc123-description"));
        assert_ne!(true, regex.is_match("task-abc-description"));
        assert_ne!(true, regex.is_match(""));
    }

    #[test]
    fn origin_extraction_works() {
        let branch_names: Vec<String> = vec![String::from("origin/a"), String::from("origin/b"), String::from("c")];
        let branch_names = extract_origin_path(branch_names);
        assert_eq!(branch_names[0], "a");
        assert_eq!(branch_names[1], "b");
        assert_eq!(branch_names[2], "c");
    }
}