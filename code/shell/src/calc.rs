use std::io;

const T_CYAN: &str = "\x1b[36m";
const T_RESET: &str = "\x1b[0m";
const T_YELLOW: &str = "\x1b[33m";
const T_RED: &str = "\x1b[31m";
const T_GREEN: &str = "\x1b[32m";
const T_BLUE: &str = "\x1b[34m";
const T_MAGENTA: &str = "\x1b[35m";

fn main() {
    println!("{}[КАЛЬКУЛЯТОР НА РАСТЕ]{}", T_CYAN, T_RESET);

    let mut input: String = String::new();

    println!("[пишите как в обычной тетрадке(к примеру 4 + 5). допустимые операции: + - * /]");

    io::stdin()
        .read_line(&mut input)
        .expect("[err]: [ошибка чтения строки]");

    let mut parts = input.split_whitespace();

    let first_num: f64 = match parts.next() {

        Some(num) => match num.parse() {
            Ok(n) => n,
            Err(_) => {
                eprintln!("{}[err]: [invalid input for the first number]{}", T_RED, T_RESET);
                return;
            }
        },

        None => {
            eprintln!("{}[err]: [no input provided for the first number]{}", T_RED, T_RESET);
            return;
        }
    };

    let symbol: char = match parts.next() {

        Some(sym) => match sym.chars().next() {

            Some(c) => c,
            None => {
                eprintln!("{}[err]: [invalid input for the symbol operation]{}", T_RED, T_RESET);
                return;
            }
        },

        None => {
            eprintln!("{}[err]: [no input provide for the fymbol operation]{}", T_RED, T_RESET);
            return;
        }
    };

    let second_num: f64 = match parts.next() {

        Some(num) => match num.parse() {

            Ok(n) => n,
            Err(_) => {
                eprintln!("{}[err]: [invalid input for the second number]{}", T_RED, T_RESET);
                return;
            }
        },

        None => {
            eprintln!("{}[err]: [no input provider for the second number]{}", T_RED, T_RESET);
            return;
        }
    };

    let res = match symbol {
        '+' => Some(first_num + second_num),
        '-' => Some(first_num - second_num),
        '*' => Some(first_num * second_num),
        '/' => Some({
            if second_num == 0.0 {
                eprintln!("{}[err]: [division by ZERO]{}", T_RED, T_RESET);
                return;
            }
            else {
                Some(first_num / second_num)
            }
        }.expect("REASON")),

        _ => {
            eprintln!("{}[err]: [invalid operation symbol]{}", T_RED, T_RESET);
            return;
        }
    };

    if let Some(reuslt) = res {
        println!("result: {:?}", res);
    }
}
