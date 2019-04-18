function isTransferEnqueued(remoteFilePath, accountWorkers) {
    for(var i = 0; i < accountWorkers.transferCommandQueue.queue.length; i++) {
        var info = accountWorkers.transferCommandQueue.queue[i].info;
        if (info.property("remoteFile") === remoteFilePath) {
            return true;
        }
    }
    return false;
}
