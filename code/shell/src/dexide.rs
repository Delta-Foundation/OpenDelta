#[path = "cmdlib/dexide_lib.rs"]
mod dexide_lib;

use dexide_lib::{
    T_GREEN, T_CYAN,
    T_RED, T_YELLOW,
    T_BLUE, T_RESET
};

use std::fs::{
    OpenOptions
};

use std::io::{
    self, 
    Write, 
    BufRead, 
    BufReader
};

const MAX_LINES: usize = 512;

pub fn main() -> io::Result<()> {
    println!("welcome to Dexide!");
    dexide_lib::editor_logo();

    let mut file_name = String::new();
    println!("input file name to edit: ");
    io::stdout().flush()?;
    io::stdin().read_line(&mut file_name)?;

    let file_name = file_name.trim();

    if file_name.is_empty() {
        eprintln!("{T_RED}[ERROR]: [File name is not be empty!]{T_RESET}");
        return Ok(());
    }

    let file = OpenOptions::new()
        .read(true)
        .write(true)
        .create(true)
        .open(file_name)?;

    let reader = BufReader::new(file);
    let mut lines: Vec<String> = reader.lines().collect::<io::Result<Vec<_>>>()?;

    loop {
        println!("[w — input the text, r — display the text, s — save, q — save and quit]");
        io::stdout().flush()?;

        let mut mode_input = String::new();
        if io::stdin().read_line(&mut mode_input)? == 0 {
            dexide_lib::save_file(file_name, &lines)?;
            break;
        }

        match mode_input.trim().chars().next().unwrap_or(' ') {
            'w' => {
                println!("{T_CYAN}[MODE]: [Inputing text; input 'E' as a separate line item, to finish]{T_RESET}");

                loop {
                    print!("{}: ", lines.len() + 1);
                    io::stdout().flush()?;

                    let mut input_line = String::new();
                    if io::stdin().read_line(&mut input_line)? == 0 {
                        break;
                    }

                    while input_line.ends_with('\n') || input_line.ends_with('\r') {
                        input_line.pop();
                    }

                    if input_line == "E" {
                        break;
                    }

                    lines.push(input_line);

                    dexide_lib::save_file(file_name, &lines)?;
                }

                dexide_lib::save_file(file_name, &lines)?;
            }
            'r' => dexide_lib::display_lines(&lines),
            's' => {
                dexide_lib::save_file(file_name, &lines)?;
                println!("{T_GREEN}[OK]: [File Saved!]{T_RESET}");
            }
            'q' => {
                dexide_lib::save_file(file_name, &lines)?;
                println!("{T_GREEN}[OK]: [File Saved!]{T_RESET}");
                break;
            }
            _ => println!("{T_RED}[ERROR]: [unknown command]{T_RESET}"),
        }
    }

    Ok(())
}
