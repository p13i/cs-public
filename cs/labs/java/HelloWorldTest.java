public final class HelloWorldTest {
    public static void main(String[] args) {
        var got = HelloWorld.sayHello("World");
        if (!"Hello World!".equals(got)) {
            throw new AssertionError("unexpected: " + got);
        }
    }
}
