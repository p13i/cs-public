using System;

namespace Labs.CSharp;

public static class Greeter
{
    public static string SayHello(string target)
    {
        return $"Hello {target}!";
    }
}

public class Program
{
    public static void Main(string[] args)
    {
        var target = args.Length > 0 ? args[0] : "World";
        Console.WriteLine(Greeter.SayHello(target));
    }
}
