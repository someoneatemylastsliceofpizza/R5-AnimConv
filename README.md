# R5-AnimConv

**R5-AnimConv** is a tool for converting `.mdl` model file to `.rrig` and `.rseq` formats. It is based on [rmdlconv](https://github.com/r-ex/rmdlconv) and [resource_model_templates](https://github.com/IJARika/resource_model_templates)

>**Note**: This tool is still in development and may contain bugs or glitches.

---

### Usage

```bash
R5-AnimConv.exe <path-to-your-model.mdl> [-nv] [-ne] [-rp <override_rrig_path>] [-sp <override_rseq_path>]"
```

>**Options**
>* `-nv` : No verbose output
>* `-ne` : No entry output
>* `-rp <override_rrig_path>` : Override internal Rrig path
>* `-sp <override_rseq_path>` : Override internal Rseq path