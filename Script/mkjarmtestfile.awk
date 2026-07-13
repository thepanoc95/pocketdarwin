#!/usr/bin/awk -f

{
    # 1. Strip the leading address (e.g., "80000c40:")
    sub(/^[^:]+:[ \t]*/, "")

    # 2. Strip the hex machine code (e.g., "e3000f58 ")
    sub(/^[0-9a-fA-F]+[ \t]*/, "")

    # 3. Strip trailing comments starting with '@'
    sub(/[ \t]*@.*/, "")

    # 4. Clean up any leftover leading/trailing spaces
    gsub(/^[ \t]+|[ \t]+$/, "")

    # 5. Print the line if it isn't empty
    if ($0 != "") {
        print $0
    }
}
