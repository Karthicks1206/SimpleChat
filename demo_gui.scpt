-- Bring a running SimpleChat window frontmost and drive the UI
on sendTo(destId, msg)
    tell application "System Events"
        set procs to (application processes whose name is "SimpleChat")
        if (count of procs) = 0 then error "SimpleChat is not running."
        set p to item 1 of procs
        set frontmost of p to true
        delay 0.2

        -- Tab to Destination field, type dest, Tab to message area, type msg, press Return
        key code 48 -- Tab
        delay 0.05
        keystroke destId
        delay 0.05
        key code 48 -- Tab
        delay 0.05
        keystroke msg
        delay 0.05
        key code 36 -- Return
    end tell
end sendTo

-- DEMO
delay 0.5
my sendTo("9002", "Hello from GUI demo to 9002")
delay 0.5
my sendTo("9004", "Hello from GUI demo to 9004")
