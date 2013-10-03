* INTRODUCTION
-----------------------------------------------------------------------
Pownload is a program that downloads and manages images from a
specific Internet site :o) It will try its best to do the "dirty"
job in an elegant and clean way. I have recently been interested in
pure C programming. This project is a playground for me to play around
some technologies that have intrigued me. For example: redis (this time
I want a serious database experience), protobuf, IPC, data structure (since
C doesn't have equivalent STL, lots of data structures need to be reinvented)
. In all, I consider this a good opportunity to pratice programming.

* INTERNAL
------------------------------------------------------------------------
The program is divided into two processes: one handles url retrieving and
management; the other controls downloading from the url and storing files
(images) inside a database and of course also does some book keepings.

