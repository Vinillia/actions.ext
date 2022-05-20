# actions.ext
Extension provides a natives to hook action event handlers and create custom actions. If two different plugins will try to return different action for same eventhandler last will be chosen. Extension doesn't support late load so after reload you must recreate nextbots. All actions are cached by their parent action event handlers

### Commands
- ext_actions_dump - dumps entities actions
- ext_actions_offsets - prints every hooked function offset 
