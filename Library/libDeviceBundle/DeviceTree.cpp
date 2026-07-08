#include "DeviceTree.hpp"
#include <regex>
#include <sstream>
#include <iostream>

bool DeviceTree::parseDTS(const std::string& dtsContent) {
    try {
        // Simple DTS parser - using a delimiter to avoid conflicts
        std::string propertyPattern = R"delim((\w+)\s*=\s*"([^"]*)";)delim";
        std::regex propertyRegex(propertyPattern);
        
        std::string currentContent = dtsContent;
        size_t pos = 0;
        
        size_t rootStart = currentContent.find("/ {");
        if (rootStart != std::string::npos) {
            pos = rootStart + 3;
            size_t rootEnd = currentContent.find("};", pos);
            
            if (rootEnd != std::string::npos) {
                std::string rootContent = currentContent.substr(pos, rootEnd - pos);
                std::smatch match;
                
                while (std::regex_search(rootContent, match, propertyRegex)) {
                    root_->addProperty(match[1].str(), match[2].str());
                    rootContent = match.suffix();
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse DTS: " << e.what() << std::endl;
        return false;
    }
}

bool DeviceTree::compileToDTB(const std::string& outputPath) const {
    std::cerr << "DTB compilation not yet implemented" << std::endl;
    return false;
}

std::string DeviceTree::compileToDTBString() const {
    return "";
}

std::shared_ptr<DeviceTree::Node> DeviceTree::findNode(const std::string& path) const {
    if (path == "/" || path.empty()) {
        return root_;
    }
    
    std::vector<std::string> pathParts;
    std::stringstream ss(path);
    std::string part;
    
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            pathParts.push_back(part);
        }
    }
    
    auto current = root_;
    for (const auto& pathPart : pathParts) {
        bool found = false;
        for (const auto& child : current->children) {
            if (child->name == pathPart) {
                current = child;
                found = true;
                break;
            }
        }
        if (!found) {
            return nullptr;
        }
    }
    
    return current;
}

std::string DeviceTree::toString() const {
    std::stringstream ss;
    ss << "/dts-v1/;\n\n";
    
    std::function<void(const std::shared_ptr<Node>&, int)> printNode = 
        [&](const std::shared_ptr<Node>& node, int indent) {
            std::string indentStr(indent * 4, ' ');
            
            if (node->name != "/") {
                ss << indentStr << node->name << " {\n";
            }
            
            for (const auto& pair : node->properties) {
                ss << indentStr << "    " << pair.first << " = \"" << pair.second << "\";\n";
            }
            
            for (const auto& child : node->children) {
                printNode(child, indent + 1);
            }
            
            if (node->name != "/") {
                ss << indentStr << "};\n";
            } else {
                ss << "};\n";
            }
        };
    
    printNode(root_, 0);
    return ss.str();
}

std::vector<std::string> DeviceTree::_tokenize(const std::string& str) const {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::shared_ptr<DeviceTree::Node> DeviceTree::_parseNode(const std::string& nodeSource) {
    auto node = std::make_shared<Node>();
    return node;
}
