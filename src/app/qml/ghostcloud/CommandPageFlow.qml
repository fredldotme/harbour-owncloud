import QtQuick 2.0

Item {
    // Inhereting items are supposed to manually connect
    // a handler to the commandFinished signal provided by
    // the accountWorker's CommandQueues
    property var accountWorkers : null
    property var directoryContents : null

    signal notificationRequest(string summary, string body)
    signal transientNotificationRequest(string summary)
    signal userInfoUpdateRequest()
    signal avatarFetchRequest()
}
