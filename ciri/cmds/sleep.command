{
    "name" : "sleep",
    "voiceTexts" : [
        "rest",
        "sleep",
        "shut up",
        "be quiet",
    ],
    "parameters" : [
        {
            "name" : "duration",
            "question" : "how many seconds?",
            "required" : 1,
            "type" : 1,
            "answer" : "sleeping for $duration seconds"
        }
    ],
    "bashCommands" : [
        {
            "cmd" : "python sleep.py $duration",
            "isSubmodule" : 1
        }
    ],
    "answer" : "i am back"
}