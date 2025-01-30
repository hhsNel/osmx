The .osmx File Format Specification

Overview: .osmx is a human-readable, structured file format designed for easy parsing and conversion into XML. It consists of a list of entries, where each entry contains specific fields: name, cost, type, main type, text, and additional metadata.

### Entry Structure

Each entry in a .osmx file follows this format:

```
<name>
	Cost: <cost>
	Type: <type>
	MainType: <main type>
	Text:
	<text>
	Power: <power> (optional)
	Toughness: <toughness> (optional)
	Loyalty: <loyalty> (optional)
	Metadata:
		<key1>: <value1>
		<key2>: <value2>
		...
```

### Field Descriptions:

- **Name**: A string that may contain spaces and punctuation.
- **Cost**: A string consisting of numbers and the characters `WUBRG/SC`.
- **Type**: A string specifying the type.
- **MainType**: A string representing the main type.
- **Text**: A multi-line string that may contain newlines.
- **Power**: An optional integer value representing power.
- **Toughness**: An optional integer value representing toughness.
- **Loyalty**: An optional integer value representing loyalty.
- **Metadata**: Optional key-value pairs where both the key and value are strings, eg. rarity.

### Example:

```
Example Card
	Cost: 2WUB
	Type: Creature - Wizard
	MainType: Creature
	Text:
	This is an example of a multi-line
	card description.
	Power: 2
	Toughness: 3
	Metadata:
		Rarity: Rare
```

### Notes:

- Each entry starts with the name on its own line.
- Fields are indented with a tab (`\t`) for readability.
- Metadata key-value pairs are indented with an additional tab for clarity.
- The `Text:` field must always be followed by a newline.
- The `Metadata:` section is optional, and any number of key-value pairs can be included.
- The format is designed for ease of parsing in C/C++ and by other AI models.

