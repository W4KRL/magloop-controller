Is it better to place Doxygen code in the header or implementation? 

Great question, Karl! The placement of Doxygen documentation depends on the purpose of the function and where it will be used. Here’s a breakdown:

### **Header (.h) vs. Implementation (.cpp)**
- **In the header file (`aprsService.h`)**:
  - Generally preferred for function declarations.
  - Makes documentation available to other files that include the header.
  - Useful for library-style code where users need to see function descriptions.

- **In the implementation file (`aprsService.cpp`)**:
  - Provides detailed documentation alongside function definitions.
  - Less clutter in the header file.
  - Best for internal functions that aren’t exposed to other modules.

Since `APRSformatBulletin()` is a function that **formats bulletin messages for APRS**, it's likely intended to be **used externally** by other parts of your project. In that case, **the Doxygen comments should be in the header file** alongside the function declaration. This allows other developers (or future-you!) to understand how to use it without diving into the implementation.

However, if the function is **internal and should not be exposed**, keeping the comments in the `.cpp` file is fine.

#### **Best Practice:**
If this function is declared in `aprsService.h` like:
```cpp
/** 
 * @brief Formats a bulletin or announcement message according to the APRS specification.
 * @param message The message content to be included in the bulletin.
 * @param ID The bulletin or announcement identifier.
 * @return Formatted APRS bulletin string.
 */
String APRSformatBulletin(String message, String ID);
```
Then the implementation (`.cpp`) wouldn’t necessarily need full Doxygen comments—just inline ones if needed.
