# Sphynx Password Checker

Sphynx is a simple checker for compromised passwords implementing the [k-anonymity model](https://blog.cloudflare.com/validating-leaked-passwords-with-k-anonymity/) of [Pwned Passwords](http://pwnedpasswords.com/).
As data source, SHA-1 hashes of known passwords are used, so the SHA-1 data files of [HIBP](https://haveibeenpwned.com/) can be utilized.

By using this checker, you can ensure users do not use known-compromised passwords, without having to use an external service. Only for refreshing the data source, an internet connection (or transfer of the new data file by other means) is required.

The API is compatible with the [range API](https://haveibeenpwned.com/API/v2#SearchingPwnedPasswordsByRange) of HIBP.

For high-performance password checks which do not require external services, the password data file is chunked into buckets of 5 byte length at build-time. The `rebuild` program used for this process should be run on a high-performance CPU with the data file and bucket folder on fast SSDs (or even in a RAM drive) for best performance. Generation of the buckets may take a while depending on the data source file and your hardware.

```bash
./rebuild password-sha1-hashes.txt buckets/
```

The supplied Apache 2.4 configuration requires the rewrite module and provides fast range lookup simply based on URL rewriting and the bucket files in the filesystem.

## Usage

The Sphynx API can be used like  the [HIBP range search](https://haveibeenpwned.com/API/v2#SearchingPwnedPasswordsByRange), without the range prefix in the URL as it only provides that API.

## Contributing

Pull requests and comments are always welcome :)
