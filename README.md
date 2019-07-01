![Maru](maru.svg)

<h2 align="center">The Maru Programming Language</h2>

<p align="center">
    <a href="https://travis-ci.com/Maximophone/maru"><img alt="Build Status" src="https://travis-ci.com/Maximophone/maru.svg?branch=master"></a>
</p>

Maru only supports integers. Because who needs floats? They only create problems...


### Known issues:
 - ILLEGAL token error if the program starts with a whitespace
 - Not really an issue but initialising a class element with an array means that the instances will share the same array
 - If a method calls itself on a different object, its "self" env variable will be overwritten