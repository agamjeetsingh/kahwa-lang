//
// Created by Agamjeet Singh on 30/12/25.
//

#ifndef LSPCONNECTION_H
#define LSPCONNECTION_H
#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>
#include "../diagnostics/DiagnosticSeverity.h"
#include "../diagnostics/Diagnostic.h"
#include "LanguageServer.h"
using json = nlohmann::json;

inline int diagnosticSeverityToInt(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::ERROR:
            return 1;
        case DiagnosticSeverity::WARNING:
        case DiagnosticSeverity::WEAK_WARNING:
            return 2;
    }
}

class LSPConnection {
public:
    LSPConnection(): log_file("/Users/agamjeetsingh/CLionProjects/kahwa-lang/kahwa_lsp_output3.txt", std::ios::app) {
        assert(log_file.is_open());

        languageServer.setConnection(this);
    }

    void run() {
        log_file << "--- LSP Server Started at " << std::time(nullptr) << " ---" << std::endl;

        // Set up standard I/O for speed (optional, but recommended)
        std::ios_base::sync_with_stdio(false);

        while (true) {
            if (std::cin.eof()) {
                log_file << "INFO: Input EOF reached. Exiting loop." << std::endl;
                break;
            }

            auto request = readMessage();
            if (request) {
                handleMessage(*request);
            } else {
                // Failed to read a message (e.g., pipe closed, bad headers)
                break;
            }
        }

        log_file << "--- LSP Server Stopped ---" << std::endl;
    }

    std::optional<json> readMessage() {
        std::string line;
        int content_length = 0;

        // Read Headers
        while (std::getline(std::cin, line)) {
            // Log the received line for debugging
            log_file << "RECEIVED: " << line << std::endl;

            // Remove trailing carriage return for consistency on all platforms
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (line.empty()) {
                // Empty line after headers marks the start of the content
                break;
            }

            // Check for Content-Length header
            if (line.rfind(CONTENT_LENGTH_HEADER, 0) == 0) {
                std::string length_str = line.substr(CONTENT_LENGTH_HEADER.length());
                try {
                    content_length = std::stoi(length_str);
                } catch (...) {
                    log_file << "ERROR: Invalid Content-Length: " << length_str << std::endl;
                    return std::nullopt;
                }
            }
        }

        if (content_length == 0) {
            // No Content-Length or stream ended
            log_file << "INFO: Input stream closed or 0 length message." << std::endl;
            return std::nullopt;
        }

        // Read Content (the JSON payload)
        std::vector<char> buffer(content_length);
        if (!std::cin.read(buffer.data(), content_length)) {
            log_file << "ERROR: Failed to read " << content_length << " bytes of content." << std::endl;
            return std::nullopt;
        }

        // Parse JSON
        try {
            std::string content(buffer.begin(), buffer.end());
            log_file << "CONTENT: " << content << std::endl;
            return json::parse(content);
        } catch (const std::exception& e) {
            log_file << "ERROR: JSON parse failed: " << e.what() << std::endl;
            return std::nullopt;
        }
    }


    void sendResponse(const json& response) {
        std::string content = response.dump();

        log_file << "SENT: " << content << std::endl;

        // 1. Send Content-Length Header
        std::cout << CONTENT_LENGTH_HEADER << content.length() << "\r\n";
        // 2. Send the empty line separating headers from content
        std::cout << "\r\n";
        // 3. Send the content
        std::cout << content;

        // 4. Flush stdout immediately so the IDE receives it.
        std::cout.flush();
    }

    void handleMessage(const json& request) {
        // A request must have a 'method' field
        if (!request.contains("method")) {
            log_file << "ERROR: Received non-request JSON." << std::endl;
            return;
        }

        const std::string method = request["method"];

        if (handlers.contains(method)) {
            handlers[method](request);
        } else {
            log_file << "INFO: Unhandled method: " << method << std::endl;
        }
    }

    void publishDiagnostics(SourceManager& sourceManager, std::vector<Diagnostic> diagnostics, std::unordered_map<std::size_t, std::string> idToUri) {
        std::vector<json> diagnostics_json;

        std::ranges::transform(diagnostics, std::back_inserter(diagnostics_json), [&sourceManager](Diagnostic& diagnostic) {
            return json{
          {
              "range", {
               {"start", {
                   {"line", sourceManager.getSourceFile(diagnostic.source_range.file_id).getLinePos(diagnostic.source_range.pos).first},
                   {"character", sourceManager.getSourceFile(diagnostic.source_range.file_id).getLinePos(diagnostic.source_range.pos).second}}},
               {"end", {
                   {"line", sourceManager.getSourceFile(diagnostic.source_range.file_id).getLinePos(diagnostic.source_range.pos + diagnostic.source_range.length).first},
                   {"character", sourceManager.getSourceFile(diagnostic.source_range.file_id).getLinePos(diagnostic.source_range.pos + diagnostic.source_range.length).second}
               }}
             }
          },
                {"severity", diagnosticSeverityToInt(diagnostic.severity)}, // 1=Error, 2=Warning, 3=Information, 4=Hint
              {"message", diagnostic.msg},
              {"source", "Kahwa LSP"}
            };
            });

        sendResponse({
            {"jsonrpc", "2.0"},
            {"method", "textDocument/publishDiagnostics"},
            {"params", {
                {"uri", idToUri[diagnostics[0].source_range.file_id]},
                {"diagnostics", diagnostics_json}
            }}});
    }

private:
    std::ofstream log_file;

    LanguageServer languageServer;

    const std::string CONTENT_LENGTH_HEADER = "Content-Length: ";
    std::unordered_map<std::string, std::function<void(const json&)>> handlers = {
        {"initialize", [this](const json& request) {
            log_file << "INFO: Handling initialize request." << std::endl;

            // --- A. Build the Capabilities ---
            // We only enable semanticTokens.
            // We use 'full: true' which is compatible with the client's 'full: {delta: false}'
            json capabilities = {
                {"textDocumentSync", {
                    {"openClose", true}, // We support didOpen/didClose
                    {"change", 1}        // We support TextDocumentSyncKind.Full (1)
                }},
                {"semanticTokensProvider", {
                    {"legend", {
                        // Minimal token types. Will expand this later.
                        {"tokenTypes", getSupportedTokenTypes()},
                        {"tokenModifiers", getSupportedTokenModifiers()}
                    }},
                    {"range", false}, // Server does not support range requests
                    {"full", true}    // Server supports full document requests (non-delta)
                }},
            };

            // --- B. Build the Response ---
            json response = {
                {"jsonrpc", "2.0"},
                {"id", request["id"]},
                {"result", {
                    {"capabilities", capabilities},
                    // Add serverInfo for logging purposes
                    {"serverInfo", {
                        {"name", "Kahwa Language Server"},
                        {"version", "1.0-alpha"}
                    }}
                }}
            };

            sendResponse(response);
        }},
        {"shutdown", [this](const json& request) {
            log_file << "INFO: Handling shutdown request. Sending response." << std::endl;
            // Send a successful response to shutdown request
            json response = {
                {"jsonrpc", "2.0"},
                {"id", request["id"]},
                {"result", json::value_t::null} // Shutdown response is typically null
            };
            sendResponse(response);
        }},
        {"exit", [this](const json& request) {
            log_file << "INFO: Handling exit notification. Exiting.\n-------------------\n" << std::endl;
            std::exit(0);
        }},
        {"textDocument/didOpen", [this](const json& request) {
            try {
                languageServer.addFile(request["params"]["textDocument"]["text"], request["params"]["textDocument"]["uri"]);
            } catch (const std::exception& e) {
                log_file << "ERROR: JSON parse failed: " << e.what() << std::endl;
                sendResponse(internalErrorMsg);
            }
        }},
        {"textDocument/didChange", [this](const json& request) {
            try {
                languageServer.addFile(request["params"]["contentChanges"][0]["text"], request["params"]["textDocument"]["uri"]);
            } catch (const std::exception& e) {
                log_file << "ERROR: JSON parse failed: " << e.what() << std::endl;
                sendResponse(internalErrorMsg);
            }
        }},
        {"textDocument/didClose", [this](const json& request) {
            try {
                languageServer.removeFile(request["params"]["textDocument"]["uri"]);
            } catch (const std::exception& e) {
                log_file << "ERROR: JSON parse failed: " << e.what() << std::endl;
                sendResponse(internalErrorMsg);
            }
        }}, {"textDocument/semanticTokens/full", [this](const json& request) {
            try {
                json json_id = request["id"];

                auto start = std::chrono::steady_clock::now(); // Just time logging

                const std::vector<int>& tokenData =
                    languageServer.syntaxHighlight(request["params"]["textDocument"]["uri"]);

                auto end = std::chrono::steady_clock::now(); // Just time logging
                auto duration_ms = // Just time logging
                    std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                log_file << "syntaxHighlight() took " << duration_ms << " ms\n"; // Just time logging

                sendResponse({
                    {"jsonrpc", "2.0"},
                    {"id", json_id},
                    {"result", {
                        {"data", tokenData}
                    }}
                });
            } catch (const std::exception& e) {
                log_file << "ERROR: JSON parse failed: " << e.what() << std::endl;
                json json_id = request["id"];
                sendResponse(getInternalErrorMsg(json_id));
            }
        }}
    };

    static json getInternalErrorMsg(const json& json_id) {
        return {
            {"jsonrpc", "2.0"},
            {"id", json_id},
            {"error", {
                {"code", -32603},
                {"message", "Internal error: Parser failed to generate AST for the document."}
            }}
        };
    }

    json internalErrorMsg = {
        {"jsonrpc", "2.0"},
        {"id", 0},
        {"error", {
            {"code", -32603},
            {"message", "Internal error: Parser failed to generate AST for the document."}
          }}
    };
};



#endif //LSPCONNECTION_H
