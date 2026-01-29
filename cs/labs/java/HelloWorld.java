public final class HelloWorld {
    public static String sayHello(String target) {
        return "Hello " + target + "!";
    }

    public static void main(String[] args) {
        var who = args.length > 0 ? args[0] : "World";
        System.out.println(sayHello(who));
    }
}
