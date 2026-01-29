#[test]
fn hello_matches() {
    let msg = hello_lib::say_hello("World");
    assert_eq!(msg, "Hello World!");
    let greeter = hello_lib::Greeter::new("Hello");
    assert_eq!(greeter.greet("World"), "Hello World");
}
