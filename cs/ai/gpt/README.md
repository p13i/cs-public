# `cs/ai/gpt/main.cc`

At `844d86fd46888a093ac3ba8743cc9b65ed418f21`:

```txt
$ make gpt
INFO: Analyzed target //cs/ai/gpt:main (0 packages loaded, 0 targets configured).
INFO: Found 1 target...
Target //cs/ai/gpt:main up-to-date:
  bazel-bin/cs/ai/gpt/main
INFO: Elapsed time: 2.886s, Critical Path: 2.60s
INFO: 3 processes: 1 action cache hit, 1 internal, 2 linux-sandbox.
INFO: Build completed successfully, 3 total actions
INFO: Running command line: bazel-bin/cs/ai/gpt/main <args omitted>
Hello
[2025-10-23T02:25:18.918287550Z] [INFO] [cs/ai/gpt/main.cc:129] Training 7-gram MKN model (forced)...
[2025-10-23T02:34:06.996462946Z] [WARNING] [cs/ai/gpt/main.cc:132] Wikipedia fetch failed; continuing.
[2025-10-23T02:34:56.497633583Z] [WARNING] [cs/ai/gpt/main.cc:132] Wikipedia fetch failed; continuing.
[2025-10-23T02:35:15.762442868Z] [WARNING] [cs/ai/gpt/main.cc:132] Wikipedia fetch failed; continuing.
[2025-10-23T02:35:58.190655406Z] [WARNING] [cs/ai/gpt/main.cc:132] Wikipedia fetch failed; continuing.
[2025-10-23T02:38:10.931969998Z] [INFO] [cs/ai/gpt/main.cc:129] Training finished in 739093 ms.
[2025-10-23T02:38:11.062529955Z] [INFO] [cs/ai/gpt/main.cc:129] Saved model to /tmp.
 occur identified entr name from which is a historic Landmark ts, a nated to curei the Hior and supp ranesian ethgnated ai Sofish enl search and sustainable useo di Cagency in the 1936 Sutics (NACA) hi Kaid and later serlin Ian of the Catholic D in some lt annah H engitalsie is a
```

---

At `ddb87191aef092960d6c639fb2a1cba0442c482b`:

```txt
$ make gpt
INFO: Analyzed target //cs/ai/gpt:main (1 packages loaded, 5 targets configured).
INFO: From Compiling cs/ai/gpt/main.cc:
cs/ai/gpt/main.cc: In member function 'void NGramLM::ApplyCacheAttention(const std::vector<unsigned char>&, std::vector<std::pair<unsigned char, float> >*, float) const':
cs/ai/gpt/main.cc:636:12: warning: variable 'ok' set but not used [-Wunused-but-set-variable]
  636 |       bool ok = true;
      |            ^~
INFO: Found 1 target...
Target //cs/ai/gpt:main up-to-date:
  bazel-bin/cs/ai/gpt/main
INFO: Elapsed time: 3.907s, Critical Path: 3.20s
INFO: 582 processes: 2 internal, 580 linux-sandbox.
INFO: Build completed successfully, 582 total actions
INFO: Running command line: bazel-bin/cs/ai/gpt/main <args omitted>
Hello
[2025-10-23T02:28:37.885537040Z] [INFO] [cs/ai/gpt/main.cc:159] Training reasoned 7-gram model (forced)...
[2025-10-23T02:34:09.455382110Z] [WARNING] [cs/ai/gpt/main.cc:162] Wikipedia fetch failed; continuing.
[2025-10-23T02:39:13.497881166Z] [WARNING] [cs/ai/gpt/main.cc:162] Wikipedia fetch failed; continuing.
[2025-10-23T02:40:07.223820095Z] [WARNING] [cs/ai/gpt/main.cc:162] Wikipedia fetch failed; continuing.
[2025-10-23T02:41:03.261212605Z] [INFO] [cs/ai/gpt/main.cc:159] Training finished in 715097 ms.
[2025-10-23T02:41:03.359199275Z] [INFO] [cs/ai/gpt/main.cc:159] Saved model to /tmp.
r of the Europe. The M and cassettte on Mountain Weaand Inforts in the College in he was also the und D under the graphical a Ro capacity of Meitary camther and a trio win The Sand the merred ter in New York City wae of the most notably oncd Massachusetts was an American ma de Gatal popular ice hockey cowry of He is th
```
