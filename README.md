# Tlockd

Small utility handling asynchronous tty/pty locking.

## Usage

```
tlockd [locker+args]
```

[locker+args] defaults to `vlock -c`

## Example

```
$ cat ~/.bashrc
if [ -z "$TLOCKD_PID" ]; then
	exec tlockd
fi
[...]
```

# `tlock`

Small wrapper to speak with tlockd
