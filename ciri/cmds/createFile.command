{
    "name" : "createFile",
    "voiceTexts" : [
        "create file",
        "create fire",
        "new file",
        "create document",
        "new document",
    ],
    "parameters" : [
        {
            "name" : "name",
            "question" : "how should i call it?",
            "required" : 1,
            "type" : 1,
            "answer" : ""
        }
    ],
    "bashCommands" : [
        {
            "cmd" : "touch $name",
            "isSubmodule" : 0
        }
    ],
    "answer" : "i created the file named $name"
}