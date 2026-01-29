package hello

import "testing"

func TestSayHello(t *testing.T) {
	got := SayHello("World")
	if got != "Hello World!" {
		t.Fatalf("unexpected output: %q", got)
	}
}
