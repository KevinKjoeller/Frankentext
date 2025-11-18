# Frankenstein Random Sentence Generator

Dette program genererer tilfældige sætninger baseret på teksten fra Mary Shelleys bog "Frankenstein" ved hjælp af en token-baseret successor table.

## Beskrivelse

Programmet analyserer teksten fra Frankenstein og bygger en datastruktur, der tracker hvilke ord (tokens) der følger efter hinanden. Derefter kan det generere nye tilfældige sætninger, der ligner den originale stil.

## Algoritme

1. **Rens tekst**: Erstat alle ikke-printbare tegn med mellemrum (`replace_non_printable_chars_with_space()`)

2. **Tokenisering** (`tokenize_and_fill_succs()`): 
   - Læs tokens adskilt af mellemrum, newline og carriage return (` \n\r`)
   - Brug `strtok()` til at opdele teksten
   - Gem tokens i et array kaldet `tokens`
   - Tokens er case-sensitive: `ruin!` og `ruin` er forskellige tokens
   - Ingen duplikater tillades

3. **Byg successor table** (`succs`):
   - For hvert token, registrér hvilke tokens der kan følge efter det
   - Bruger `append_to_succs()` til at tilføje successor-relationer
   - `token_id()` finder eller opretter tokens

4. **Generér tilfældige sætninger** (`generate_sentence()`):
   - Vælg et tilfældigt token, der starter med stort bogstav (`random_token_id_that_starts_a_sentence()`)
   - Tilføj tilfældige successorer fra successor tabellen
   - Fortsæt indtil vi støder på et token, der ender en sætning (`.`, `!`, `?`)
   - Stop også hvis sætningen bliver for lang (1000 tegn)

5. **Hovedprogram**:
   - Generér sætninger indtil vi finder én der slutter med `?`
   - Generér sætninger indtil vi finder én der slutter med `!`

## Fil struktur

```
Franekntext.c              - Hovedprogrammet med implementering
README_Frankenstein.md     - Denne fil
```

## Kompilering og kørsel

### Windows (PowerShell med clang)

```powershell
clang Franekntext.c -o Franekntext.exe
./Franekntext.exe
```

### Alternative compilere

```powershell
# Med gcc
gcc Franekntext.c -o Franekntext.exe

# Med MSVC
cl Franekntext.c
```

## Eksempel på output

Programmet genererer to sætninger hver gang det køres:

```
More miserable than man ever was before, why did I thought of his country; a husband and rest?

Nothing is so painful to the most distinguished of life that he became a husband and sudden change!
```

## Datastrukturer

### Tokens array
- Array af alle unikke tokens fra teksten
- Case-sensitive (stort/småt betyder noget)
- Ingen duplikater

### Successor table
- For hvert token gemmes en liste af mulige efterfølgende tokens
- Bruges til at generere nye sætninger ved at følge tilfældige stier

## Token regler

- **Token delimiters**: mellemrum, punktum (`.`), udråbstegn (`!`), spørgsmålstegn (`?`)
- **Sætnings-start**: Tokens der starter med stort bogstav
- **Sætnings-slut**: Tokens der ender med `.`, `!`, eller `?`
- **Case-sensitivity**: `Ruin` ≠ `ruin` ≠ `ruin!`

## Tekniske noter

- Programmet bruger dynamisk hukommelse til at håndtere variabelt antal tokens
- Successor table implementeres typisk som en adjacency list eller hash table
- Tilfældighed styres af `rand()` og `srand()`

## Videre forbedringer

- Tilføj support for bi-gram eller tri-gram modeller (to eller tre tokens som kontekst)
- Implementér Markov chain med højere orden
- Gem og genindlæs successor table for hurtigere opstart
- Tilføj parametre for sætningslængde eller stil
- Support for andre bøger/tekster

## License

Til uddannelsesformål. Frankenstein-teksten er public domain.
