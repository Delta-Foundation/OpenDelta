use std::thread;
use std::time::Duration;
use std::time::{SystemTime, UNIX_EPOCH};

fn main() {
    loop {
        let now = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs();
        let (time_str, date_str) = format_moscow_time(now);
        print_clock(&time_str, &date_str);
        thread::sleep(Duration::from_secs(1));
        print!("\x1B[2J\x1B[1;1H");
    }
}

fn format_moscow_time(timestamp: u64) -> (String, String) {
    let moscow_time = timestamp + 3 * 3600;
    let hours = (moscow_time % 86400) / 3600;
    let minutes = (moscow_time % 3600) / 60;
    let seconds = moscow_time % 60;
    let days_since_epoch = moscow_time / 86400;
    let year = 1970 + days_since_epoch / 365;
    let day = days_since_epoch % 365;
    let month = (day as f64 / 30.44).floor() as u64 + 1;
    let day = (day as f64 % 30.44).floor() as u64 + 1;

    let time_str = format!("{:02}:{:02}", hours, minutes);
    let date_str = format!("{:02}.{:02}.{}", day, month, year);
    (time_str, date_str)
}

fn print_clock(time: String, date: String) {
    println!("{}", " ".repeat(5));
    println!("{}", time.chars().map(|c| big_char(c)).collect::<String>());
    println!("{}", date);
}

fn big_char(c: char) -> String {
    match c {
        '0' => {
            "  ███ \n"
            " █   █\n"
            " █   █\n"
            " █   █\n"
            "  ███ \n"
        }

        '1' => {
            " ████ \n"
            "   ██  \n"
            "   ██  \n"
            "   ██  \n"
            "  ████ \n"
        }

        '2' => {
            " ████ \n"
            "     █\n"
            "  ███ \n"
            " █    \n"
            "  ████\n"
        }

        '3' => {
            " ███ \n"
            "    █\n"
            " ███ \n"
            "    █\n"
            " ███ \n"
        }

        '4' => {
            " █   █\n"
            " █   █\n"
            "  ████\n"
            "     █\n"
            "     █\n"
        }

        '5' => {
            "  ████\n"
            " █    \n"
            "  ███ \n"
            "     █\n"
            "  ████\n"
        }

        '6' => {
            "  ███ \n"
            " █    \n"
            "  ███ \n"
            " █   █\n"
            "  ███ \n"
        }

        '7' => {
            "  ████\n"
            "     █\n"
            "     █\n"
            "     █\n"
            "     █\n"
        }

        '8' => {
            "  ███ \n"
            " █   █\n"
            "  ███ \n"
            " █   █\n"
            "  ███ \n"
        }

        '9' => {
            "  ███ \n"
            " █   █\n"
            "  ████\n"
            "     █\n"
            "  ███ \n"
        }

        ':' => {
            "      \n"
            "   █  \n"
            "      \n"
            "   █  \n"
            "      \n"
        }

        _ => "     \n" "    \n" "    \n" "    \n" "    \n",
    }
    .to_string()
}
