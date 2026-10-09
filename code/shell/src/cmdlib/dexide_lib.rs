use std::fs::{
    File, OpenOptions
};

use std::io::{
    self, 
    Write, 
    BufRead, 
    BufReader
};

pub const T_CYAN: &str = "\x1b[36m";
pub const T_RESET: &str = "\x1b[0m";
pub const T_YELLOW: &str = "\x1b[33m";
pub const T_RED: &str = "\x1b[31m";
pub const T_GREEN: &str = "\x1b[32m";
pub const T_BLUE: &str = "\x1b[34m";
pub const T_MAGENTA: &str = "\x1b[35m";

pub fn editor_logo() {
    println!("{}████████        ███████     █       █   ██  ████████        ███████    {}", T_MAGENTA, T_RESET);
    println!("{}██     ██      ██     ██    ██     ██       ██     ██      ██     ██   {}", T_MAGENTA, T_RESET);
    println!("{}██      ██    ██       ██    ██   ██    ██  ██      ██    ██       ██  {}", T_MAGENTA, T_RESET);
    println!("{}██       ██  ██         ██    █████     ██  ██       ██  ██         ██ {}", T_MAGENTA, T_RESET);
    println!("{}██       ██  █████████████   ██   ██    ██  ██       ██  █████████████ {}", T_MAGENTA, T_RESET);
    println!("{}██      ██    ██            ██     ██   ██  ██      ██    ██           {}", T_MAGENTA, T_RESET);
    println!("{}█████████      ██████████   █       █   ██  █████████      ██████████  {}", T_MAGENTA, T_RESET);
}

pub fn display_file(file_name: &str) -> io::Result<()> {
    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    for line in reader.lines() {
        println!("{}", line?);
    }

    Ok(())
}

pub fn save_file(file_name: &str, lines: &[String]) -> io::Result<()> {
    let mut file = File::create(file_name)?;

    for line in lines {
        writeln!(file, "{line}")?;
    }

    Ok(())
}

pub fn display_lines(lines: &[String]) {
    for line in lines {
        println!("{line}");
    }
}
