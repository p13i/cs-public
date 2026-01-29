function sayHello(target) {
  return `Hello ${target}!`;
}

function main() {
  const who = process.argv[2] ?? "World";
  console.log(sayHello(who));
}

if (require.main === module) {
  main();
}

module.exports = { sayHello };
