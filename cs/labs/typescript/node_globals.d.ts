interface NodeModuleLike {
  exports: unknown;
  main?: NodeModuleLike;
}

interface NodeRequireLike {
  main?: NodeModuleLike;
  (path: string): unknown;
}

declare const require: NodeRequireLike;
declare const module: NodeModuleLike;
declare const process: { argv: string[] };
