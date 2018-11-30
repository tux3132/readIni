<h1>readIni</h1>
<p>readIni is a programmer's library in C++ to read ".ini" MS Windows style files in Linux systems. readIni takes little resources and offers functions for sequentially browse or search directly by key. An examples file in C++ is provided.</p>

<h1>Features</h1>
<ul>
<li>You may use a colon to separate key and value. The colon is equivalent to the equal sign. That is, the strings "Name:Value" and "Name=Value" have the same meaning.</li>
<li>The hash character ("#") is an alternative for the semicolon to start a comment. Trailing comments (i.e. behind a key/value pair on a line) are allowed.</li>
<li>Leading and/or trailing white space around key names and values are ignored.</li>
<li>When writing a value that contains a comment character (";" or "#"), that value will automatically be put between double quotes; when reading the value, these quotes are removed. When a double-quote itself appears in the setting, these characters are escaped.</li>
<li>The memory footprint is deterministic. There is no dynamic memory allocation.</li>
</ul>

<h1>.ini file syntax</h1>
<p>.ini files have a simple syntax with name/value pairs in a plain text file. The name must be unique (per section) and the value must fit on a single line. .ini files are  separated into sections, a section is a name between square brackets, like "[Network]" in the example below.</p>
<pre>
<code>[Network]
hostname=My Computer
address=dhcp # This is a comment
dns = 192.168.1.1
</code>
</pre>

<h1>Acknowledgement</h1>
<p>readIni is derived from the project minIni of Compubase. Please see <a href="https://github.com/compuphase/minIni" rel="nofollow">https://github.com/compuphase/minIni</a></p>
