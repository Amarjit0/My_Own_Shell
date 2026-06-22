-- KaliShell Script: Basic Reconnaissance Pipeline
-- This script demonstrates the security DSL concept

-- Set target
target = "example.com"

-- Phase 1: Subdomain Discovery
print("[*] Phase 1: Subdomain Discovery")
run("subfinder", "-d " .. target .. " -o subdomains.txt")

-- Phase 2: HTTP Probing
print("[*] Phase 2: HTTP Probing")
run("httpx", "-l subdomains.txt -o live_hosts.txt -sc -title")

-- Phase 3: Endpoint Discovery
print("[*] Phase 3: Endpoint Discovery")
run("katana", "-u " .. target .. " -o endpoints.txt -d 3")

-- Phase 4: Vulnerability Scanning
print("[*] Phase 4: Vulnerability Scanning")
run("nuclei", "-l live_hosts.txt -o findings.json -severity critical,high,medium")

-- Summary
print("[+] Reconnaissance complete!")
print("    Subdomains: " .. count_lines("subdomains.txt"))
print("    Live Hosts: " .. count_lines("live_hosts.txt"))
print("    Endpoints: " .. count_lines("endpoints.txt"))
