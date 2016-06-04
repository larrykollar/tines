<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" />
<xsl:strip-space elements="*" />

<!--	hnb to Groff -ms
		Version 1.0, 2016 Jan 23 by Larry Kollar

		This transform uses the "text" marker to tell the difference
		between headings and paragraph text. Text entries under another
		text entry are rendered as bullet list items.

<xsl:template match="/tree">
		<xsl:apply-templates />
</xsl:template>
-->

<!-- ******************************************************************
	Process the body
-->

<xsl:template match="node[not(@type)]">
	<!-- assume it's a heading; use top-level as a title -->
	<xsl:choose>
		<xsl:when test="count(ancestor::node)=0">.nr PS 12p
.nr VS 14p
.nr PSINCR 2p
.nr GROWPS 4
.RP no
.TL
<xsl:value-of select="./data/text()" />
.AU
[author name here]
.AI
[author institution here]
</xsl:when>
<xsl:when test="count(ancestor::node)=1">
.NH 1
<xsl:value-of select="./data/text()" />
</xsl:when>
<xsl:when test="count(ancestor::node)=2">
.NH 2
<xsl:value-of select="./data/text()" />
</xsl:when>
<xsl:when test="count(ancestor::node)=3">
.NH 3
<xsl:value-of select="./data/text()" />
</xsl:when>
<xsl:when test="count(ancestor::node)=4">
.NH 4
<xsl:value-of select="./data/text()" />
</xsl:when>
<xsl:otherwise>
.SH 5
<xsl:value-of select="./data/text()" />
</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="node" />
</xsl:template>

<!--
	type="text" is paragraphs
-->
<xsl:template match="node[@type='text']">
	<xsl:choose>
		<!-- text under text is bulleted list items -->
		<xsl:when test="parent::node[@type='text']">
.IP \[bu] 4
<xsl:value-of select="./data/text()" />
		</xsl:when>
		<xsl:otherwise>
.LP
<xsl:value-of select="./data/text()" />
		</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="node" />
</xsl:template>

<!-- type="todo" is also paragraph, but with a leading marker -->
<xsl:template match="node[@type='todo']">
	<xsl:choose>
		<xsl:when test="current()[@done='yes']">
.LP
\[u2713] <xsl:value-of select="./data/text()" />
</xsl:when>
<xsl:otherwise>
.LP
\[u2610] <xsl:value-of select="./data/text()" />
</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="node" />
</xsl:template>

</xsl:stylesheet>
