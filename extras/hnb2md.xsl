<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" />
<xsl:strip-space elements="*" />

<!--	hnb to Markdown
		Version 1.0, 2016 Jan 23 by Larry Kollar

		This transform uses the "text" marker to tell the difference
		between headings and paragraph text. Text entries under another
		text entry are rendered as bullet list items.
-->

<xsl:template match="/tree">
		<xsl:apply-templates />
</xsl:template>

<!-- ******************************************************************
	Process the body
-->

<xsl:template match="node[not(@type)]">
	<!-- assume it's a heading -->
	<xsl:choose>
<xsl:when test="count(ancestor::node)=0"># <xsl:value-of select="./data/text()" />
<xsl:text>
</xsl:text>
</xsl:when>
<xsl:when test="count(ancestor::node)=1">## <xsl:value-of select="./data/text()" />
<xsl:text>
</xsl:text>
</xsl:when>
<xsl:when test="count(ancestor::node)=2">### <xsl:value-of select="./data/text()" />
<xsl:text>
</xsl:text>
</xsl:when>
<xsl:when test="count(ancestor::node)=3">#### <xsl:value-of select="./data/text()" />
<xsl:text>
</xsl:text>
</xsl:when>
<xsl:when test="count(ancestor::node)=4">##### <xsl:value-of select="./data/text()" />
<xsl:text>
</xsl:text>
</xsl:when>
<xsl:otherwise>###### <xsl:value-of select="./data/text()" />
<xsl:text>
</xsl:text>
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
* <xsl:value-of select="./data/text()" />
<xsl:text>
</xsl:text>
		</xsl:when>
		<xsl:otherwise>
<xsl:value-of select="./data/text()" />
<xsl:text>

</xsl:text>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="node" />
</xsl:template>

<!-- type="todo" is also paragraph, but with a leading marker -->
<xsl:template match="node[@type='todo']">
	<xsl:choose>
	<xsl:when test="current()[@done='yes']">&#2713; <xsl:value-of select="./data/text()" />
<xsl:text>

</xsl:text>
</xsl:when>
	<xsl:otherwise>&#2610; <xsl:value-of select="./data/text()" />
<xsl:text>

</xsl:text>
</xsl:otherwise>
	</xsl:choose>
	<xsl:apply-templates select="node" />
</xsl:template>

</xsl:stylesheet>
