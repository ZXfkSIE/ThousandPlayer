java -jar ~/bfg-1.14.0.jar --strip-blobs-bigger-than 1M --delete-files '*.{png,svg,ts,zip,yml}' && git reflog expire --expire=now --all && git gc --prune=now --aggressive
