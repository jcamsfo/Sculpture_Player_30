// fs_utils.cpp
#include "fs_utils.h"
#include <filesystem>
#include <system_error>
namespace fs = std::filesystem;

// #include <unistd.h>   // fork, execlp
// #include <sys/wait.h> // waitpid
// #include <signal.h>   // kill, SIGTERM, SIGKILL



// ---- Drop-in replacement ----
Path_Parsed Parse_Filename_Path(const std::string& Full_Path_In)
{
    Path_Parsed Path{};
    Path.errors = -1;

    if (Full_Path_In.empty()) return Path;

    fs::path p(Full_Path_In);

    // Path_Only (with trailing separator if non-empty, like your original)
    if (p.has_parent_path()) {
        Path.Path_Only = p.parent_path().string();
        if (!Path.Path_Only.empty() && Path.Path_Only.back() != fs::path::preferred_separator)
            Path.Path_Only.push_back(fs::path::preferred_separator);
    } else {
        Path.Path_Only.clear();
    }

    // Filename, stem, extension (extension without leading dot)
    Path.Filename         = p.filename().string();
    Path.Filename_No_Ext  = p.stem().string();

    Path.Ext = p.has_extension() ? p.extension().string() : std::string{};
    if (!Path.Ext.empty() && Path.Ext.front() == '.') Path.Ext.erase(0, 1);

    Path.errors = 0;
    return Path;
}

// Finds a file anywhere in the initial_path or any sub folder
std::string Find_File(const std::string& initial_path, const std::string& filename)
{
    std::error_code ec;
    if (!fs::exists(initial_path, ec) || !fs::is_directory(initial_path, ec))
        return "";

    // Be tolerant of unreadable dirs; skip permission errors instead of throwing
    fs::directory_options opts = fs::directory_options::skip_permission_denied;

    for (fs::recursive_directory_iterator it(initial_path, opts, ec), end;
         it != end; it.increment(ec))
    {
        if (ec) continue;                       // skip entries that error out
        const fs::directory_entry& de = *it;
        if (!de.is_regular_file(ec)) continue;  // only compare files
        if (de.path().filename() == filename)
            return de.path().string();
    }
    return "";
}



// Finds a folder in the initial_path DOES NOT check subfolders
std::string Find_Folder(const std::string& initial_path,
                        const std::string& foldername)
{
    std::error_code ec;

    if (!fs::exists(initial_path, ec) || !fs::is_directory(initial_path, ec))
        return "";

    for (const auto& de : fs::directory_iterator(initial_path, ec))
    {
        if (de.is_directory(ec) &&
            de.path().filename() == foldername)
        {
            return de.path().string();
        }
    }

    return "";
}


// Creates a folder if it's not there!  does nothing if iut already exists
bool Create_Folder(const std::string& parent_path,
                   const std::string& foldername)
{
    std::error_code ec;

    fs::path new_folder = fs::path(parent_path) / foldername;

    if (fs::exists(new_folder, ec))
        return true;    // already exists

    return fs::create_directory(new_folder, ec);
}




bool Create_Text_File_If_Missing(const std::string& filename,
                                 const std::string& contents)
{
    if (fs::exists(filename))
        return true;

    std::ofstream out(filename);

    if (!out)
        return false;

    out << contents;
    return true;
}