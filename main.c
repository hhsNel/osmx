#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_LINE 1024
#define MAX_ENTRIES 100

#define LOG(msg) if(verbose) printf("\t[v]  " msg);
#define LOGX(msg, ...) if(verbose) printf("\t[v+] " msg, __VA_ARGS__);

typedef struct Metadata {
	char key[MAX_LINE];
	char value[MAX_LINE];
	struct Metadata *next;
} Metadata;

typedef struct {
	char name[MAX_LINE];
	char cost[MAX_LINE];
	char type[MAX_LINE];
	char mainType[MAX_LINE];
	char text[MAX_LINE * 2];
	char power[MAX_LINE];
	char toughness[MAX_LINE];
	char loyalty[MAX_LINE];
	Metadata *metadata;  // Linked list of metadata

} Entry;

void add_metadata(Metadata **head, const char *key, const char *value, int verbose) {
	LOGX("Adding metadata: %s\n", key);
	Metadata *new_entry = malloc(sizeof(Metadata));
	if (!new_entry) return;
	strncpy(new_entry->key, key, MAX_LINE);
	strncpy(new_entry->value, value, MAX_LINE);
	new_entry->next = *head;
	*head = new_entry;
}

char *get_metadata(Metadata *head, const char *key) {
	while (head) {
		if (strcmp(head->key, key) == 0) return head->value;
		head = head->next;
	}
	return NULL;  // Not found
}

void edit_metadata(Metadata *head, const char *key, const char *new_value) {
    while (head) {
        if (strcmp(head->key, key) == 0) {
            strncpy(head->value, new_value, MAX_LINE);
            return;
        }
        head = head->next;
    }
}

void delete_metadata(Metadata **head, const char *key) {
	Metadata *curr = *head, *prev = NULL;
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			if (prev) prev->next = curr->next;
			else *head = curr->next;
			free(curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

Entry entries[MAX_ENTRIES];
int entry_count = 0;

#include "render.h"

void parse_osmx(const char *filename, int verbose);
int search_entries(const char *query, int start_index);
void prompt_user();
void write_xml(FILE *file, const char *set_name, const char *longname, const char *release_date, int verbose);
int calculate_cmc(const char *cost, int verbose);
void get_unique_colors(const char *cost, char *colors, int verbose);
void render_cards();

int main(int argc, char **argv) {
	char input_file[MAX_LINE], output_file[MAX_LINE];
	input_file[0] = output_file[0] = '\0';
	char set_name[MAX_LINE], longname[MAX_LINE], release_date[MAX_LINE];
	
	int render_flag = 0, edit_flag = 1, output_flag = 0, verbose_flag = 0;
	for(int i = 1; i < argc; ++i) {
		if(argv[i][0] == '-') {
			for(char *opt = argv[i]+1; *opt; ++opt) {
				switch(*opt) {
					case 'r':
						render_flag = 1;
						break;
					case 'R':
						render_flag = 0;
						break;
					case 'e':
						edit_flag = 1;
						break;
					case 'E':
						edit_flag = 0;
						break;
					case 'v':
						verbose_flag = 1;
						break;
					case 'V':
						verbose_flag = 0;
						break;
					case 'i':
						if(i + 1 >= argc) {
							printf("Expected another argument after -i\n");
							exit(1);
						}
						strcpy(input_file, argv[++i]);
						goto next_argument;
					case 'o':
						if(i + 1 >= argc) {
							printf("Expected another argument after -o\n");
							exit(1);
						}
						strcpy(output_file, argv[++i]);
						goto next_argument;
					case 'n':
						if(i + 1 >= argc) {
							printf("Expected another argument after -n\n");
							exit(1);
						}
						if(strcmp(argv[++i], "file") == 0) {
							output_flag = 0;
						} else if(strcmp(argv[i], "stdout") == 0) {
							output_flag = 1;
						} else if(strcmp(argv[i], "none") == 0) {
							output_flag = 2;
						} else {
							printf("Expected file, stdout or none after -n");
							exit(1);
						}
						goto next_argument;
				}
			}
		}
		next_argument:
	}
	
	if(!input_file[0]) {
		printf("Enter the .osmx filename: ");
		scanf("%s", input_file);
	}
	parse_osmx(input_file, verbose_flag);
	
	if(edit_flag) {
		prompt_user();
	}
	
	if(!output_file[0] && output_flag == 0) {
		printf("Enter output .xml filename: ");
		scanf("%s", output_file);
	}
	printf("Enter set name: ");
	scanf(" %[^\n]s", set_name);
	printf("Enter long name: ");
	scanf(" %[^\n]s", longname);
	printf("Enter release date (or press Enter for today's date): ");
	while (getchar() != '\n'); // Clear input buffer
	fgets(release_date, MAX_LINE, stdin);
	if (release_date[0] == '\n') {
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		sprintf(release_date, "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	}
	
	if(output_flag == 0) {
		FILE *file = fopen(output_file, "w");
		if(file) {
			write_xml(file, set_name, longname, release_date, verbose_flag);
		} else {
			printf("Cannot open file %s\n", output_file);
			exit(1);
		}
	} else if(output_flag == 1) {
		write_xml(stdout, set_name, longname, release_date, verbose_flag);
	}
	
	if(render_flag) render_cards();
	return 0;
}

void parse_osmx(const char *filename, int verbose) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(1);
	}

	char line[MAX_LINE], key[MAX_LINE], value[MAX_LINE];
	Entry *current = NULL;
	int reading_text = 0;

	while (fgets(line, MAX_LINE, file)) {
		if (line[0] != '\t' && line[0] != '\n') {
			LOG("New entry\n");
			// New entry
			current = &entries[entry_count++];
			LOGX("Number of entries: %d\n", entry_count);
			strcpy(current->name, line);
			LOGX("Entry name: %s\n", current->name);
			current->text[0] = '\0';
			current->power[0] = '\0';
			current->toughness[0] = '\0';
			current->loyalty[0] = '\0';
			reading_text = 0;
		} else {
			if(line[0] == '\t' && line[1] == '\t') {
				char key[MAX_LINE], value[MAX_LINE];
				LOG("Metadata detected\n");
				if (sscanf(line + 2, "%[^:]: %[^\n]", key, value) == 2) {
					add_metadata(&current->metadata, key, value, verbose);
				}
			} else if (reading_text && strcmp(line, "\tMetadata:\n") != 0) {
				// If it's part of text, append with a newline for readability
				strcat(current->text, line + 1);  // Skip the tab character
				LOGX("Read text: %s\n", line);
			} else if (sscanf(line, "\t%[^:]: %[^\n]", key, value) == 2) { 
				if (strcmp(key, "Cost") == 0) {
					strcpy(current->cost, value);
					LOGX("Cost: %s\n", current->cost);
					reading_text = 0;
				} else if (strcmp(key, "Type") == 0) {
					strcpy(current->type, value);
					LOGX("Type: %s\n", current->type);
					reading_text = 0;
				} else if (strcmp(key, "MainType") == 0) { 
					strcpy(current->mainType, value);
					LOGX("Main type: %s\n", current->mainType);
					reading_text = 0;  // Ensure text does not start here
				} else if (strcmp(key, "Power") == 0) {
					strcpy(current->power, value);
					LOGX("Power: %s\n", current->power);
					reading_text = 0;
				} else if (strcmp(key, "Toughness") == 0) {
					strcpy(current->toughness, value);
					LOGX("Toughness: %s\n", current->toughness);
					reading_text = 0;
				} else if (strcmp(key, "Loyalty") == 0) {
					strcpy(current->loyalty, value);
					LOGX("Loyalty: %s\n", current->loyalty);
					reading_text = 0;
				}
			} else if (strcmp(line, "\tText:\n") == 0) {
				reading_text = 1;  // Start capturing multi-line text
				LOG("Started reading text\n");
			} else if (strcmp(line, "\tMetadata:\n") == 0) {
				reading_text = 0;
			}
		}
	}
	fclose(file);
}

void print_metadata(Metadata *head) {
	if (!head) {
		printf("No metadata.\n");
		return;
	}

	printf("Metadata:\n");
	while (head) {
		printf("\t%s: %s\n", head->key, head->value);
		head = head->next;
	}
}

void print_entry(Entry entry) {
	printf("\nEntry: %s\nCost: %s\nType: %s\nMainType: %s\nText: %s\n",
		entry.name, entry.cost, entry.type, entry.mainType, entry.text);
	if (strlen(entry.power) > 0 && strlen(entry.toughness) > 0) {
		printf("Power/Toughness: %s/%s\n", entry.power, entry.toughness);
	}
	if (strlen(entry.loyalty) > 0) {
		printf("Loyalty: %s\n", entry.loyalty);
	}
	print_metadata(entry.metadata);
}

int search_entries(const char *query, int start_index) {
	for (int i = 0; i < entry_count; i++) {
		int index = (start_index + i) % entry_count;

		if (strstr(entries[index].name, query) || strstr(entries[index].text, query)) {
			return index;
		}
	}

	return -1;  // No match found
}

int reverse_search_entries(const char *query, int start_index) {
	for (int i = 0; i < entry_count; i++) {
		int index = (start_index + entry_count - i) % entry_count;

		if (strstr(entries[index].name, query) || strstr(entries[index].text, query)) {
			return index;
		}
	}

	return -1;  // No match found
}

void bulk_replace_text(Entry *entries, int num_entries, const char *old_word, const char *new_word) {
	for (int i = 0; i < num_entries; i++) {
		char *pos, buffer[MAX_LINE * 4];  // Buffer to store updated text
		char *src = entries[i].text;
		char *dst = buffer;

		while ((pos = strstr(src, old_word))) {
			size_t len_before = pos - src;
			strncpy(dst, src, len_before);
			dst += len_before;
			strcpy(dst, new_word);
			dst += strlen(new_word);
			src = pos + strlen(old_word);
		}
		strcpy(dst, src);  // Copy remaining text
		strcpy(entries[i].text, buffer); // Save changes
	}
}

void prompt_user() {
	int i = 0;
	char last_search[MAX_LINE];
	while(1) {
		print_entry(entries[i]);
		printf("(A)pprove, (R)eject, (E)dit, (S)earch, (Q)uit editor, (B)ulk replace, (M)etadata edit? ");
		char choice;
		scanf(" %c", &choice);
		getchar();
		int found;
		switch(choice) {
			case 'Q':
			case 'q':
				return;
			case 'A':
			case 'a':
			case 'j':
				++i;
				break;
			case 'k':
				--i;
				break;
			case 'R':
			case 'r':
				printf("Entry rejected.\n");
				for (int j = i; j < entry_count - 1; j++) {
					entries[j] = entries[j + 1];
				}
				entry_count--;
				break;
			case 'E':
			case 'e':
				while (1) {
					printf("\nEdit (N)ame, (C)ost, (T)ype, (M)ain Type, (P)ower, To(U)ghness, (L)oyalty, Te(X)t, (V)iew, (D)one: ");
					char edit_choice;
					scanf(" %c", &edit_choice);
					getchar();
					char buffer[MAX_LINE];
					
					if (edit_choice == 'N' || edit_choice == 'n') {
						fgets(entries[i].name, MAX_LINE, stdin);
					} else if (edit_choice == 'C' || edit_choice == 'c') {
						fgets(entries[i].cost, MAX_LINE, stdin);
					} else if (edit_choice == 'T' || edit_choice == 't') {
						fgets(entries[i].type, MAX_LINE, stdin);
					} else if (edit_choice == 'M' || edit_choice == 'm') {
						fgets(entries[i].mainType, MAX_LINE, stdin);
					} else if (edit_choice == 'P' || edit_choice == 'p') {
						fgets(entries[i].power, MAX_LINE, stdin);
					} else if (edit_choice == 'U' || edit_choice == 'u') {
						fgets(entries[i].toughness, MAX_LINE, stdin);
					} else if (edit_choice == 'L' || edit_choice == 'l') {
						fgets(entries[i].loyalty, MAX_LINE, stdin);
					} else if (edit_choice == 'X' || edit_choice == 'x') {
						printf("Enter new Text. Type 'exit' or an empty line twice to finish:\n");
						entries[i].text[0] = '\0';
						while (1) {
							fgets(buffer, MAX_LINE, stdin);
							if (strcmp(buffer, "exit\n") == 0) break;
							if (buffer[0] == '\n') {
								fgets(buffer, MAX_LINE, stdin);
								if (buffer[0] == '\n') break;
								strcat(entries[i].text, "\n");
							}
							strcat(entries[i].text, buffer);
						}
					} else if (edit_choice == 'V' || edit_choice == 'v') {
						print_entry(entries[i]);
					} else if (edit_choice == 'D' || edit_choice == 'd') {
						++i;
						break;
					}
				}
				break;
			case 'S':
			case 's':
			case '/':
				printf("Enter substring search query: ");
				scanf(" %[^\n]s", last_search);
			case 'n':
				found = search_entries(last_search, (i + 1) % entry_count);

				if (found != -1) {
					i = found;
				} else {
					printf("No matching entries found.\n");
				}
				break;
			case 'N':
				found = reverse_search_entries(last_search, (i - 1 + entry_count) % entry_count);

				if (found != -1) {
					i = found;
				} else {
					printf("No matching entries found.\n");
				}
				break;
			case 'B':
			case 'b':
				char old_word[MAX_LINE], new_word[MAX_LINE];

				printf("Find: ");
				fgets(old_word, MAX_LINE, stdin);
				old_word[strcspn(old_word, "\n")] = '\0'; // Remove newline

				printf("Replace with: ");
				fgets(new_word, MAX_LINE, stdin);
				new_word[strcspn(new_word, "\n")] = '\0'; // Remove newline

				bulk_replace_text(entries, entry_count, old_word, new_word);
				printf("Replaced all occurrences of '%s' with '%s'.\n", old_word, new_word);
				break;
			case 'M':
			case 'm':
					char key[MAX_LINE], value[MAX_LINE];
					printf("Enter metadata key: ");
					fgets(key, MAX_LINE, stdin);
					key[strcspn(key, "\n")] = '\0';

					printf("Enter new value: ");
					fgets(value, MAX_LINE, stdin);
					value[strcspn(value, "\n")] = '\0';

					if (get_metadata(entries[i].metadata, key)) {
						edit_metadata(entries[i].metadata, key, value);
					} else {
						add_metadata(&entries[i].metadata, key, value, 0);
					}
		}
	}
}

void write_xml(FILE *file, const char *set_name, const char *longname, const char *release_date, int verbose) {
	fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(file, "<cockatrice_carddatabase version=\"4\">\n");
	fprintf(file, "  <sets>\n  <set>\n");
	fprintf(file, "  <name>%s</name>\n", set_name);
	fprintf(file, "  <longname>%s</longname>\n", longname);
	fprintf(file, "  <settype>Custom</settype>\n");
	fprintf(file, "  <releasedate>%s</releasedate>\n", release_date);
	fprintf(file, "</set>\n  </sets>\n  <cards>\n");
	
	for (int i = 0; i < entry_count; i++) {
		fprintf(file, "  <card>\n");
		fprintf(file, "	<name>%s</name>\n", entries[i].name);
		fprintf(file, "	<text>%s</text>\n", entries[i].text);
		fprintf(file, "	<prop>\n");
		fprintf(file, "	  <type>%s</type>\n", entries[i].type);
		fprintf(file, "	  <maintype>%s</maintype>\n", entries[i].mainType);
		fprintf(file, "	  <manacost>%s</manacost>\n", entries[i].cost);
		fprintf(file, "	  <cmc>%d</cmc>\n", calculate_cmc(entries[i].cost, verbose));
		char colors[MAX_LINE] = "";
		get_unique_colors(entries[i].cost, colors, verbose);
		fprintf(file, "	  <colors>%s</colors>\n", colors);
		fprintf(file, "	  <coloridentity>%s</coloridentity>\n", colors);
		if (strlen(entries[i].power) > 0 && strlen(entries[i].toughness) > 0) {
			fprintf(file, "	  <pt>%s/%s</pt>\n", entries[i].power, entries[i].toughness);
		}
		if (strlen(entries[i].loyalty) > 0) {
			fprintf(file, "	  <loyalty>%s</loyalty>\n", entries[i].loyalty);
		}
		fprintf(file, "	</prop>\n");
		fprintf(file, "    <set>%s</set>\n", set_name);
		fprintf(file, "  </card>\n");
	}
	
	fprintf(file, "  </cards>\n</cockatrice_carddatabase>\n");
	fclose(file);
}

int calculate_cmc(const char *cost, int verbose) {
	LOGX("Mana cost: %s\n", cost);
	int cmc = 0;
	while (*cost) {
		if (isdigit(*cost)) {
			cmc += *cost - '0';
		} else if (strchr("WUBRGSC", *cost)) {
			cmc++;
		} else if (*cost == '/') {
			cmc--;
		}
		cost++;
	}
	LOGX("Calculated CMC: %d\n", cmc);
	return cmc > 0 ? cmc : 0; // Ensure CMC is never negative
}

void get_unique_colors(const char *cost, char *colors, int verbose) {
	LOGX("Mana cost: %s\n", cost);
	int color_mask = 0;
	const char *order = "WUBRG"; // Define the order of colors
	char *ptr = colors;

	while (*cost) {
		switch (*cost) {
			case 'W': color_mask |= 1 << 0; break;
			case 'U': color_mask |= 1 << 1; break;
			case 'B': color_mask |= 1 << 2; break;
			case 'R': color_mask |= 1 << 3; break;
			case 'G': color_mask |= 1 << 4; break;
		}
		cost++;
	}

	for (int i = 0; i < 5; i++) {
		if (color_mask & (1 << i)) {
			*ptr++ = order[i];
		}
	}

	*ptr = '\0'; // Null-terminate the string

	if (ptr == colors) {
		strcpy(colors, "C"); // Default to colorless if no colors found
	}
	LOGX("Unique colors: %s\n", colors);
}

void render_cards() {
	printf("Rendering cards...\n");
	Image img;
	for(int i = 0; i < entry_count; ++i) {
		char filename[MAX_LINE];
		strcpy(filename, entries[i].name);
		filename[strcspn(filename, "\n")] = '\0';
		strcat(filename, ".ff");
		printf(" >> Rendering %s...\n", filename);
		render_card(&img, entries[i]);
		save_farbfeld(filename, &img);
		printf(" >> %s rendered.\n", filename);
	}
	printf("Cards rendered.\n");
}
