/*
  переписываю dexide из dltsh с си на растик.
*/
use std::fs::File;
use std::io::{self, Write, BufRead, BufReader};

const MAX_LINES: usize = 512;

const T_CYAN: &str = "\x1b[36m";
const T_RESET: &str = "\x1b[0m";
const T_YELLOW: &str = "\x1b[33m";
const T_RED: &str = "\x1b[31m";
const T_GREEN: &str = "\x1b[32m";
const T_BLUE: &str = "\x1b[34m";
const T_MAGENTA: &str = "\x1b[35m";

pub fn editor_logo() {
    println!("{}████████        ███████     █       █   ██  ████████        ███████    {}", T_MAGENTA, T_RESET);
    println!("{}██     ██      ██     ██    ██     ██       ██     ██      ██     ██   {}", T_MAGENTA, T_RESET);
    println!("{}██      ██    ██       ██    ██   ██    ██  ██      ██    ██       ██  {}", T_MAGENTA, T_RESET);
    println!("{}██       ██  ██         ██    █████     ██  ██       ██  ██         ██ {}", T_MAGENTA, T_RESET);
    println!("{}██       ██  █████████████   ██   ██    ██  ██       ██  █████████████ {}", T_MAGENTA, T_RESET);
    println!("{}██      ██    ██            ██     ██   ██  ██      ██    ██           {}", T_MAGENTA, T_RESET);
    println!("{}█████████      ██████████   █       █   ██  █████████      ██████████  {}", T_MAGENTA, T_RESET);
}

fn display_file(file_name: &str) -> io::Result<()> {
    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    for line in reader.lines() {
        println!("{}", line?);
    }

    Ok(())
}

pub fn main() -> io::Result<()> {
    println!("welcome to Dexide!");
    editor_logo();

    let mut lines: Vec<String> = Vec::new(); //: [[char; MAX_LINE_LENGTH]; MAX_LINES];
    let mut file_name = String::new();
    let mut mode_input: String = String::new();
    let mut line_count: usize = 0;

    println!("input file name to edit: ");
    io::stdout().flush()?;
    io::stdin()
        .read_line(&mut file_name)
        .expect("[err]: [failed to read file name]");
    file_name = file_name.trim().to_string();

    let file = File::open(&file_name)?;
    let reader = BufReader::new(file);

    for line in reader.lines() {
        if line_count >= MAX_LINES {
            break;
        }
        lines.push(line?);
        line_count += 1;
    }

    loop {
        println!("[enter 'w' for input, 'r' for read, 'q' for exit]: ");
        io::stdout().flush()?;

        mode_input.clear();
        io::stdin()
            .read_line(&mut mode_input)
            .expect("[err]: [failed to read mode]");
        
        let mode = mode_input.trim().chars().next().unwrap_or(' ');

        if mode == 'q' {
            break;
        }

        if mode == 'w' {
            println!("{}[start input (enter 'E' for exit input)]: \n{}", T_CYAN, T_RESET);
        
            while line_count < MAX_LINES {
                print!("{}: ", line_count + 1);
                io::stdout().flush()?;

                let mut input_line = String::new();
                match io::stdin().read_line(&mut input_line) {
                    Ok(_) => {
                        input_line = input_line.trim().to_string();

                        if input_line.is_empty() || input_line == "E" {
                            break;
                        }

                        lines.push(input_line);
                        line_count += 1;

                        if line_count >= MAX_LINES {
                            println!("{}[warn]: [the maximum number of lines has been reached. Complete the entry]{}", T_YELLOW, T_RESET);
                            break;
                        }
                    }
                    Err(_) => {
                        eprintln!("{}[err]: [failed to read line.]{}", T_RED, T_RESET);
                        break;
                    }
                }
            }
        }

        if mode == 'r' {
            display_file(&file_name)?;
        }
    }

    Ok(())
}
