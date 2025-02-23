#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include <QStack>
#include <QQmlEngine>

#include <QQuickView>
#include <QtQml>

// Local Metadata Database
#include <localdb/imagedatabase.h>
#include <localdb/databasestatistic.h>
#include <localdb/lastfmalbumprovider.h>
#include <localdb/qmlimageprovider.h>

// MPD Connection
#include <mpd/mpdoutput.h>
#include <mpd/mpdtrack.h>
#include <mpd/networkaccess.h>
#include <mpd/serverprofile.h>
#include <mpd/artistmodel.h>
#include <mpd/albummodel.h>
#include <mpd/filemodel.h>
#include <mpd/playlistmodel.h>
#include <mpd/serverprofilemodel.h>
#include <mpd/mpdplaybackstatus.h>

#include <player.h>


class Controller : public QObject
{

    enum LastFMDownloadSizes {
        LASTFM_SMALL,
        LASTFM_MEDIUM,
        LASTFM_LARGE,
        LASTFM_EXTRALARGE,
        LASTFM_MEGA
    };

    Q_OBJECT
    Q_PROPERTY(Player* player READ player CONSTANT)

public:
    explicit Controller(QObject *parent = nullptr);
    Controller(QQuickView *mQuickView, QObject *parent = nullptr);
    ~Controller();
    void connectSignals();

    Player* player() const
    {
        return m_player;
    }

public slots:

signals:
    void sendPopup(QVariant text);
    void sendStatus(QVariant status);
    void playlistUpdated();
    void filesModelReady();
    void getFiles(QString path);
    void requestConnect();
    void requestDisconnect();
    void albumsReady();
    void artistsReady();
    void albumTracksReady();
    void artistAlbumsReady();
    void savedPlaylistsReady();
    void savedPlaylistReady();
    void outputsReady();
    void serverProfilesUpdated();
    void setUpdateInterval(int);
    void showWelcome();
    void requestExit();
    void searchedTracksReady();
    void addURIToPlaylist(QString);
    void requestPlaylistClear();
    void requestArtistAlbumMap();
    void requestArtists();
    void filePopCleared();
    void connected(QVariant profile);
    void disconnected();

    void requestCoverArt(MpdAlbum album);
    void requestCoverArtistArt(MpdArtist artist);

    void requestArtistImageFill(QList<MpdArtist*>*);
    void requestAlbumFill(QMap<MpdArtist*, QList<MpdAlbum*>* > *);

    void requestDBStatistic();
    void newDownloadSize(QString);
    void newDownloadEnabled(bool);

    /*
     * Workaround signals for QJSValue->QVariant cast crashes
     */
    void requestAlbum(QVariant);
    //void addAlbum(QVariant);
    //void playAlbum(QVariant);
    void requestSearch(QVariant);
    void requestAlbumInfo(QVariant);
    void addSongToSaved(QVariant);
    void removeSongFromSaved(QVariant);


private:
    QQuickView *mQuickView;
    NetworkAccess *mNetAccess;
    QString mHostname,mPassword,mProfilename;
    quint16 mPort;
    bool mWasConnected;
    QTimer mReconnectTimer;
    quint32 mPlaylistVersion;
    //int mCurrentSongID;
    int mVolume;
    int mLastPlaybackState;
    QThread *mNetworkThread;
    QThread *mDBThread;
    ServerProfileModel *mServerProfiles;
    //FIXME what are they for?:
    //QTimer volDecTimer,volIncTimer;
    AlbumModel *mOldAlbumModel;
    ArtistModel *mOldArtistModel;
    //PlaylistModel *mPlaylist;
    PlaylistModel *mOtherTracks;
    QStringList *mSavedPlaylists;
    QList<MPDOutput*> *mOutputs;
    QStack<FileModel*> *mFileModels;
    ImageDatabase *mImgDB;
    QMLImageProvider *mQMLImgProvider;
    bool mApplicationActive;

    //MPDPlaybackStatus *mPlaybackStatus;

    //DB
    DatabaseStatistic *mDBStatistic;
    int mDownloadSize;
    int mDownloadEnabled;
    QString getLastFMArtSize(int index);

    // GUI Settings
    int mAlbumViewSetting;
    int mArtistViewSetting;
    int mListImageSize;
    int mSectionsInSearch;
    int mSectionsInPlaylist;
    int mArtistsViewUseAlbumArtist;
    int mCoverInNowPlaying;
    int mShowModeLandscape;
    int mShowPositionSlider;
    int mShowVolumeSlider;
    int mRemorseTimerSecs;
    int mUseVolumeRocker;
    int mStopMPDOnExit;

    void readSettings();
    void writeSettings();

    Player* m_player;

private slots:
    void requestFilePage(QString);
    //void seek(int);
    //FIXME what are they for?:
    //void incVolume();
    //void decVolume();
    /*Privates*/
    void connectedToServer();
    void disconnectedToServer();
    void updateAlbumsModel(QList<QObject*>* list);
    void updateArtistsModel(QList<QObject*>* list);
//    void updateArtistAlbumsModel(QList<QObject*>* list);
    void updateFilesModel(QList<QObject*>* list);
    void updateOutputsModel(QList<QObject*>* list);
    void setHostname(QString mHostname);
    void setPassword(QString mPassword);
    void setPort(int mPort);
    void connectToServer();
    void quit();
    void newProfile(QVariant profile);
    void changeProfile(QVariant profile);
    void deleteProfile(int index);
    void connectProfile(int index);
    void updateSavedPlaylistsModel(QStringList*);
    void fileStackPop();
    void cleanFileStack();
    void exitRequest();
    void addlastsearchtoplaylist();

    void focusChanged(QObject *now);

    void clearAlbumList();
    void clearArtistList();
    void clearPlaylists();
    void clearTrackList();

    void reconnectServer();

    void fillArtistImages();
    void fillArtistImages(QList<QObject*>*);

    void fillAlbumImages();

    void newDBStatisticReceiver(DatabaseStatistic *statistic);

    void setArtistBioInfo(QString info);
    void setAlbumWikiInfo(QString info);

    void receiveDownloadSize(int);
    void receiveSettingKey(QVariant setting);

    void trimCache();

    void wakeUpHost(int index);

    void onNewAlbum();
    void onNewArtist();

    /*
     * Workaround slots for QJSValue->QVariant cast crashes
     */
    void getAlbumTracks(QVariant album);
    //void addArtistAlbumToPlaylist(QVariant album);
    //void playArtistAlbum(QVariant album);
    void searchTracks(QVariant search);
    void requestAlbumWikiInformation(QVariant album);
    void addTrackToSavedPlaylist(QVariant);
    void removeTrackFromSavedPlaylist(QVariant);

};

#endif // CONTROLLER_H
