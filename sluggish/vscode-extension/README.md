# Sluggish for VS Code

Basic lexical highlighting for `.slug` files: `return`, `word_8`, `word_16`,
decimal numbers, identifiers, operators and punctuation. Braces and parentheses
use VS Code's bracket matching and bracket colors. Colors follow your theme.

This is a [TextMate grammar](https://code.visualstudio.com/api/language-extensions/syntax-highlight-guide)
with no runtime JavaScript, dependencies or language server.

To package and install locally, run from this directory:

```sh
mkdir -p ../build
npx --yes @vscode/vsce@4.0.0 package --no-dependencies --allow-missing-repository --skip-license --out ../build/sluggish-vscode.vsix
code --install-extension ../build/sluggish-vscode.vsix --force
```

Open a `.slug` file; its language mode should be **Sluggish**. If an already-open
file does not update, run **Developer: Reload Window** from the Command Palette.

Edit `syntaxes/sluggish.tmLanguage.json` as lexer tokens are added. Only `return`
is currently a keyword; comments and strings are not part of the language yet.
After editing, repeat the package/install commands and reload the window.
