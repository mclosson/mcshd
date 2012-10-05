mcshd
=====

Multicast Shell Daemon and Client

TODO:

* Presently output from commands is sent inefficiently calling sendto() once for each line of shell output.
* Presently there is no verification of who responded to a command so spoofing responses is very easy.
* An identification mechanism needs to be built into the communication protocol to identifier the responder.
* Add support for encrypted communications on top of the plain text multicast traffic
* Add support for steganographic communication, hiding the commands in an audio stream or something

Usage:

<pre>
$ ./mcshd 225.31.33.7 5000
$ ./client
mc#: <command>
</pre>
