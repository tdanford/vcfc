vcfc
====

An experimental VCF flattener in C

Question: how fast can we parse / flatten records out of VCF files?  

Answer: with the current implementation, looks like about ~2ms/row on a decent machine, 
which means parsing all of 1KG in a single thread in 60 hours.  (For reference, a reasonable
Python VCF parser was taking about 20ms/row.) 
