# Text-Analyser
This is a program that will stem and POS tag words from a sentence to a given dictionary. An example follows below:

#allocate
vt
$0allocated1allocated2allocating
#sell
vt vi n
$0sold1sold2selling3sells
#she
n pron
$
#the
art
$
#zebra
n
$3zebra
%%%%%%%%%%
she sells seashells

In the input above, the dictionary starts with a # and ends with the 10 % signs. The sentence follows below the % line.
The program will read in the dictionary and then the words in the sentence. Following that, it will stem and POS tag the words if they are found in the dictionary.
