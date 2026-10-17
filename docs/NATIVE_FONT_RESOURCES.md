# Native Linux font resources

The canonical MB Corpo faces are shared machine resources on Linux rather than files owned by any individual application package.

The native location is:

```text
/System/Resources/Fonts/
    mb_corpo_a_cond_regular.ttf
    mb_corpo_s_bold.ttf
    mb_corpo_s_regular.ttf
```

`cmake/InfiltratrEnsureNativeFonts.sh` is the canonical installer-side bootstrap. A first-party Linux installer may copy that script from its pinned Common source and execute it with root privileges during installation. The bootstrap verifies any existing resources by SHA-256, downloads the immutable MBLink archive only when a resource is missing or invalid, verifies the archive and all three extracted faces, and replaces invalid resources atomically.

The bootstrap also establishes `/etc/fonts/conf.d/90-infiltrator-native-fonts.conf`, which adds `/System/Resources/Fonts` to Fontconfig. GTK/Pango applications therefore continue to select the canonical family names rather than carrying product-specific font paths.

## Ownership and lifecycle

The resource files and Fontconfig registration are deliberately not application-package payload. They are machine state established by any Infiltrator installer that needs the typography contract. This avoids multiple Debian packages claiming the same pathname.

Application uninstall and purge logic must not delete `/System/Resources/Fonts`, its canonical faces, or the Infiltrator Fontconfig registration. Removing one application must never break another application. A future explicit system-level cleanup/reset operation may remove shared native resources when that behaviour is intentionally designed and qualified.

InfiltratorOS may pre-populate the same canonical location. On such systems, application installers verify the existing files and perform no download when the hashes already match.

On a Linux system without InfiltratorOS, the first Infiltrator installer establishes the directory and resources. Later installers reuse the same verified files. If the resources have been damaged or replaced, any installer using the canonical bootstrap repairs them from the pinned source.

## Package and platform boundary

Linux application packages should not redistribute extra copies under `/usr/share/fonts/truetype/<application>` or bundle the MB archive merely to seed Linux fonts. The package may contain the small bootstrap logic copied from its exact pinned Common revision.

A first installation onto a Linux machine that does not already contain the resources requires HTTPS access to the pinned MBLink archive and either `curl` or `wget`; archive extraction requires `tar` with xz support. Installers should declare or otherwise satisfy those requirements.

This Linux native-resource contract does not force Windows, Apple or other platforms to use `/System`. Platform-native releases may continue to embed/register the same verified faces using the mechanisms appropriate to that platform.
