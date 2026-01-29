using System;
using Labs.CSharp;

namespace Labs.CSharp;

public class HelloWorldTest
{
    public static int Main()
    {
        var result = Greeter.SayHello("World");
        var expected = "Hello World!";
        
        if (result != expected)
        {
            Console.Error.WriteLine($"Expected: '{expected}', Got: '{result}'");
            return 1;
        }
        
        Console.WriteLine("Test passed!");
        return 0;
    }
}

