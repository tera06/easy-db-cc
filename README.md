# Software Design
# Class Design
- repl Directory
    - Repl Class
        - public
            - Status start();
        - private
            - Status print_prompt();
            - std::pair<std::unique_ptr\<ReplUserInput\>,Status> read_user_input();
            - Status print_user_output(ReplUserOutput&)
            - Status print_row(Row&);
        
    - ReplUserInput Class
    - ReplUserOutput Class


- command Directory
    - UserInput Class
    - UserOutput Class
    - Command Class
        - public
            - std::pair<std::unique_ptr\<UserOutput\>,Status> execute(UserInput&);
    - CommandParser Class
        -  std::pair<std::unique_ptr\<CommandStatement\>,Status> parse(UserInput&)
    - CommandExecutor
        -  std::pair<std::unique_ptr\<UserOutput\>,Status> execute(CommandStatement&)
    - CommandStatement

- database Directory
    - Database Class
        - std::pair<std::unique_ptr\<Table\>,Status> open(String);
    - Table
        - Status insert(Row&)
        - vector<Row&> select_all()
    - btree SubDirectory
    - page SubDirectory

- Status Directory
    - Status Class
        - bool is_ok();
        - bool is_err();



    





