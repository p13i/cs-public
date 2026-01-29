export function sayHello(target: string): string {
  return `Hello ${target}!`;
}

function main(): void {
  const who = process.argv[2] ?? "World";
  console.log(sayHello(who));
}

if (require.main === module) {
  main();
}
