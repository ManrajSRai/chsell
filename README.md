# chsell
### Overview

This C program, named `cshell`, simulates a simple shell environment allowing the user to interact with a custom command-line interface. The program can be run in interactive mode or script mode (using `myscript.txt`), and supports various built-in commands and functionalities, such as setting environment variables, changing text colors, executing print commands, and logging command history.

### Key Features

1. **Command Parsing**: The `split_tokens` function takes user input and splits it into separate tokens, which are then processed based on the command type.

2. **Environment Variables**: Users can create and modify custom environment variables using `$variable=value` syntax. These variables are stored in an array and can be referenced in other commands.

3. **Color Themes**: The program allows changing the text color of the shell using the `theme` command. Supported colors include red, blue, green, and the default terminal color.

4. **Print Functionality**: The `print` command outputs the content of environment variables or any string directly to the console.

5. **Logging**: Every command entered gets logged with a timestamp and return value. The `log` command can be used to display the history of executed commands.

6. **Error Handling**: The shell provides error messages for unrecognized commands, incorrect input formats, or when attempting to reference non-existing environment variables.

7. **Script Mode**: The program can execute commands from a script file, specifically `myscript.txt`. This mode reads and executes commands sequentially from the file.

8. **Exit Function**: Allows the user to exit the shell using the `exit` command, which also logs the action before shutting down the program.

9. **System Call Integration**: Includes a custom system call (simulated in code as a feature) for converting strings to uppercase, showcasing how system-level operations might be integrated.

10. **External Command Execution**: The shell can execute non-built-in commands by searching for the executable in the system path and running it in a separate process, capturing its output.

### Usage

- **Interactive Mode**: Simply run the executable without any arguments to start the shell in interactive mode. You will be prompted to enter commands directly.
- **Script Mode**: Run the executable with a single argument pointing to a valid script file (e.g., `./cshell myscript.txt`). The shell will execute commands from the script.

### Commands

- **`print [string|$variable]`**: Prints the string or the value of the environment variable to the terminal.
- **`$variable=value`**: Sets or updates an environment variable.
- **`theme [red|blue|green|default]`**: Changes the shell's text color.
- **`log`**: Displays the log of all executed commands along with their timestamps and outcomes.
- **`exit`**: Exits the shell and ends the session.
- **`uppercase [string]`**: Converts the provided string to uppercase using a simulated system call.

### Additional Information

- **Error Handling**: The shell provides robust error handling for various scenarios, such as invalid commands, file access issues, and incorrect input formats.
- **Expandability**: The shell's structured design allows for easy addition of new commands and features.
