# R5-AnimConv

**R5-AnimConv** is a tool for converting `.mdl` model file to `.rrig` and `.rseq` formats. It is based on [rmdlconv](https://github.com/r-ex/rmdlconv), [RSX](https://github.com/r-ex/rsx) and [resource_model_templates](https://github.com/IJARika/resource_model_templates)

---

### Usage

```bash
Mdl  mode : R5-AnimConv.exe <model.mdl> [-verbose] [-ne] [-rp <override_rrig_path>] [-sp <override_rseq_path>]
```
```bash
Rseq mode : R5-AnimConv.exe <parent directory> [-i <in season>] [-o <out season>] [-verbose] [-ne]
```

>**Options**
>* `-i <in_season>`  : Input assets season (default: 23)
>* `-o <out_season>` : Output assets season (default: 3)
>* `-verbose`    : Verbose outputs
>* `-ne`         : No RePak Entries outputs
>* `-skipevents` : Skip sequence events
>* `-nopause`    : No pause at the end of execution
>* `-rp <override_rrig_path>` : Override internal rrig path
>* `-sp <override_rseq_path>` : Override internal rseq path
จ