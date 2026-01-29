pub fn say_hello(target: &str) -> String {
    format!("Hello {}!", target)
}

pub struct Greeter {
    greeting: String,
}

impl Greeter {
    pub fn new(greeting: &str) -> Greeter {
        Greeter { greeting: greeting.to_string() }
    }

    pub fn greet(&self, thing: &str) -> String {
        format!("{} {}", &self.greeting, thing)
    }
}
