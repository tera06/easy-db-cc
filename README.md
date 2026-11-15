# Software Design
# Class Design
- repl Directory
    - Repl Class
        - public
            - Status start();
        - private
            - Status print_prompt();
            - Result\<UserInput\> read_user_input();
            - Status print_user_output(UserOutput&);
            - using UserInput = std::string;
            - using UserOutput = std::string;


- command Directory
    - Command Class
        - public
            - Result\<CommandResult\> execute(const UserCommand&);
            - using UserCommand = std::string;
            - using  CommandResult= std::string;
        - private
            CommandParser parser_;
            CommandExecutor executor_;
            
    - CommandParser Class
        - public
            -  Result\<std::unique_ptr\<CommandStatement\>\> parse(const ParseTarget&)
            - using ParseTarget = std::string;
    - CommandExecutor Class
        - public
            - Result\<CommandResult\> execute(const CommandStatement&)
            - using CommandResult = std::string;
    - CommandStatement Class
        - public
            - enum Class StatementType statement_type_;
            - Row insert_row;

The command layer currently supports `INSERT <id> <username> <email>` and
`SELECT` (or `SELECT *`). `Command` is constructed with an initialized `Table`
and returns command output as a string.

- database Directory
    - Database Class
        - public
            - static Result\<std::unique_ptr\<Table\>\> open(const String&);
    - Table Class
        - public
            - Result<void> initialize();
            - Result<void> insert(const Row&);
            - Result<vector<Row>> select_all();
            - Result<void> flush();
        - private
            - std::unique_ptr\<Btree\> btree_;
    - Row Class
        - public
            - id_t get_id() const;
    - btree SubDirectory
        - Btree Class
            - public
                - Btree(const std::string& filename);
                - Result\<void\> initialize();
                - Result\<void\> insert(key_t key, const value_t& value);
                - Result\<value_t\> find(key_t key) const;
                - Result\<std::vector\<value_t\>\> get_all() const;
                - Result\<void\> flush();
                - using key_t = LeafNode::key_t;
                - using value_t = LeafNode::value_t;
            - private
                - std::unique_ptr\<PageManager\> page_manager_;
                - PageManager::page_id_t root_page_id_;
        - LeafNode Class
            - public
                - page_内部のgetter及びsetter
            - private
                - const Page& page_;
        - InternalNode Class
            - public
                - page_内部のgetter及びsetter
            - private 
                - const Page& page_;
    - page SubDirectory
        - PageManager Class
            - public
                - using PageId = std::uint32_t;
                - Result<Page&>　get_page(PageId);
                - Status flush(PageId);
            - private
                - const std::string filename;
                - std::vector\<std::unique_ptr\<Page\>\> pages;
        - Page Class
            - public 
                - std::array<std::byte, size> data_;

- Status Directory
    - Status Class
        - bool is_ok();
        - bool is_err();
    - template \<typename T \> using Result = std::pair<T, Status>



    


