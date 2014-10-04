libradius-linux
===============

LINUX port of recent (juniper)-libradius offered by FreeBSD.

Cause... I needed a lightweight and simple to integrate solution for a small project.

There is a previous port available at http://portal-to-web.de/tacacs/libradius.php, but
- it is a little bit outdated (2004, referenced pages are currently not available),
- needs libdm to be installed and
- does not use OpenSSL libs if present.

I decided to use a more recent version from the FreeBSD git repo (f1d6f47/ Sep 21 2014) - applied changes:
- removed / replaced FreeBSD preprocessor magic,
- replaced BSD specific sranddev by srand(gettimeofday),
- ships with an included md5 lib (http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5) that is used if OpenSSL libs are not present (+ use common MD5_* function names) and
- use CMAKE based build.

Use it at your own risk and have phun!

hecke
